#include <cstring>

#include "PathMover.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::UI;
using namespace RoninEngine::AI;

constexpr int MaxPopulation = 2000;
constexpr int InitPopuplation = 100;
constexpr int NavMeshMagnitude = 100;
constexpr float NavMeshWorldScale = 0.32;

uid g_buttons;
uid b_mainMenu;
uid b_generateNav;
uid b_clearNav;
uid b_showGizmo;
uid t_infoblock;
uid t_speedLabel;

NavMesh *population_navmesh = nullptr;
NavListSite paths;

Vec2 from, to;
bool showGizmosLayer = true;
int arranged = 0;
Neuron *last = nullptr;
float changeDrawPointPer;
decltype(paths.end()) _endIter;
MoveController2D *player;

void ui_event_handler(uid id, void *userData)
{
    if(b_mainMenu == id)
    {
        RoninSimulator::LoadWorld(RoninMemory::alloc<PathMoverMenu>());
    }
    else if(b_generateNav == id || id == b_clearNav)
    {
        paths.clear();
        arranged = 0;
        if(b_generateNav == id)
            population_navmesh->GenerateMaze();
        else
            population_navmesh->Clear(true);
    }
    else if(b_showGizmo == id)
    {
        showGizmosLayer = !showGizmosLayer;
    }
}

void PathMover::OnAwake()
{
    Resolution res = RoninSimulator::GetCurrentResolution();

    GameObject *playerObj = create_game_object("Player");
    player = playerObj->AddComponent<MoveController2D>();

    player->playerCamera->visibleBorders = true;

    g_buttons = getGUI()->PushGroup();
    //  b_mainMenu = get_gui()->push_button("Main Menu", Vec2Int(25, 25), nullptr, g_buttons);
    Rect pos = {250, 0, 60, 16};
    b_generateNav = getGUI()->PushButton("generate", pos, nullptr, g_buttons);
    pos.x += 60;
    b_clearNav = getGUI()->PushButton("clear", pos, nullptr, g_buttons);
    pos.x += 60;
    b_showGizmo = getGUI()->PushButton("Gizmos", pos, nullptr, g_buttons);
    pos.x += 60;
    t_infoblock = getGUI()->PushLabel("", pos, 5);
    pos.x += 60;
    t_speedLabel = getGUI()->PushLabel("", pos, 5);
    getGUI()->SetGeneralCallback(ui_event_handler, nullptr);
}

SpriteRenderer *player_target;
GameObject *testObj;
std::vector<std::tuple<Transform *, float, NavListSite>> populations;

Neuron *get_free_neuron(Neuron *from)
{

    if(from && population_navmesh->hasLocked(from))
    {
        NavListNeuron neurons = population_navmesh->GetNeighbours(NavIdentity::DiagonalMethod, from);

        for(int x = 0; x < neurons.size(); ++x)
        {
            if(!population_navmesh->hasLocked(neurons[x]))
                return neurons[x];
        }
    }

    return from;
}

void PathMover::OnStart()
{
    SpriteRenderer *spriteRenderer;
    Sprite *test_sprite, *floor_sprite, *target_sprite;

    switch_game_level(this);

    const std::string sprite_dir = "./data/";
    test_sprite = Primitive::create_sprite2d_from(Resources::GetImageSource(Resources::LoadImage(sprite_dir + "TestSprite.png", true)));
    floor_sprite = Primitive::create_sprite2d_from(Resources::GetImageSource(Resources::LoadImage(sprite_dir + "floor.png", true)));
    target_sprite = Primitive::create_sprite2d_from(Resources::GetImageSource(Resources::LoadImage(sprite_dir + "target.png", true)));

    testObj = create_game_object("Population");
    testObj->transform()->position(Vec2::zero);
    testObj->transform()->layer(1); // layer

    spriteRenderer = testObj->AddComponent<SpriteRenderer>();
    spriteRenderer->renderType = SpriteRenderType::Simple;
    spriteRenderer->renderPresentMode = SpriteRenderPresentMode::Fixed;
    spriteRenderer->flip = {1, 1};
    spriteRenderer->set_sprite(test_sprite);

    GameObject *floor = create_game_object("Floor");
    floor->transform()->layer(-100);
    spriteRenderer = floor->AddComponent<SpriteRenderer>();
    spriteRenderer->renderType = SpriteRenderType::Tile;
    spriteRenderer->renderPresentMode = SpriteRenderPresentMode::Fixed;
    spriteRenderer->size = {NavMeshMagnitude, NavMeshMagnitude};
    spriteRenderer->size *= NavMeshWorldScale;
    spriteRenderer->set_sprite(floor_sprite);
    floor->SetActive(false);

    RoninMemory::alloc_self(population_navmesh, static_cast<std::size_t>(NavMeshMagnitude), static_cast<std::size_t>(NavMeshMagnitude));
    population_navmesh->worldScale = Vec2::one * NavMeshWorldScale;
    population_navmesh->GenerateMaze();

    changeDrawPointPer = 0;
    player_target = create_game_object()->AddComponent<SpriteRenderer>();
    player_target->transform()->layer(1);
    player_target->set_sprite(target_sprite);

    NavListSite points;
    for(size_t i = 0, y = Math::Min(MaxPopulation, InitPopuplation); i < y; ++i)
    {
        auto t = Instantiate(testObj)->transform();
        auto neuron = population_navmesh->PointToWorldPoint(Random::Range(0, NavMeshMagnitude), Random::Range(0, NavMeshMagnitude));

        t->position(neuron);
        populations.emplace_back(std::make_tuple(t, 0, points));
    }
}

void move_targets(int max, int &activeCount);

void PathMover::OnUpdate()
{
    static int activeCount = 0;
    if(Camera::mainCamera() == nullptr)
    {
        return;
    }

    Vec2 p = Camera::mainCamera()->ScreenToWorldPoint(Input::GetMousePointf());
    Neuron *selNeuron = get_free_neuron(population_navmesh->get(p));
    from = testObj->transform()->position();

    if(Input::GetKeyDown(KB_ESCAPE))
    {
        if(!getGUI()->GetElementVisible(g_buttons))
            getGUI()->GroupShow(g_buttons);
        else
            getGUI()->GroupClose(g_buttons);
    }

    if(TimeEngine::frame() % 25 == 0)
    {
        std::string s = Input::GetKeyDown(KB_LSHIFT) ? "Speed x2" : "Speed x1";
        s.push_back(';');
        s += "Count ";
        s += std::to_string(populations.size());
        s += " Active ";
        s += std::to_string(activeCount);
        getGUI()->SetElementText(t_speedLabel, s);
    }
    if(selNeuron && selNeuron != last)
    {
        char p[70];
        p[0] = '\0';
        if(selNeuron->f)
        {
            std::strcat(p, "cost: ");
            std::strcat(p, std::to_string(selNeuron->g).data());
            std::strcat(p, "; ");
            std::strcat(p, "h: ");
            std::strcat(p, std::to_string(selNeuron->h).data());
            std::strcat(p, "; ");
            getGUI()->SetElementVisible(t_infoblock, true);
            getGUI()->SetElementText(t_infoblock, p);
        }
        else
            getGUI()->SetElementVisible(t_infoblock, false);

        last = selNeuron;
    }
    if(player_target)
    {
        if(!getGUI()->FocusedGUI() && Input::GetMouseUp(MouseState::MouseLeft))
        {
            p = Input::GetMousePointf();
            p = Camera::mainCamera()->ScreenToWorldPoint(p);
            paths.clear();
            auto a = get_free_neuron(population_navmesh->get(from));
            auto b = get_free_neuron(population_navmesh->get(p));

            testObj->transform()->position(population_navmesh->NeuronToWorldPoint(a));

            if(b)
            {
                player_target->transform()->position(population_navmesh->NeuronToWorldPoint(b));
            }

            population_navmesh->Clear();

            NavResultSite result;
            if(!population_navmesh->Find(result, a, b))
            {
                // todo: error
                arranged = 0;
                paths.clear();
            }
            else
            {
                paths = std::move(result.connections);
                arranged = 1;
                _endIter = begin(paths);
            }
            population_navmesh->Clear();
        }
    }

    if(!paths.empty())
    {
        int j;
        if(_endIter != std::end(paths) && TimeEngine::time() > changeDrawPointPer)
        {
            changeDrawPointPer = TimeEngine::time() + 0.001f;
            for(j = 0; _endIter != std::end(paths) && j < paths.size() / Math::Sqrt(paths.size()); ++j, ++_endIter)
                ;
        }

        Vec2 lp = population_navmesh->PointToWorldPoint(*begin(paths));
        auto iter = paths.cbegin();
        for(j = 0; iter != _endIter && j <= arranged; ++iter, ++j)
            lp = population_navmesh->PointToWorldPoint(*iter);
        to = lp;

        if(arranged && _endIter == std::end(paths))
        {
            if(from != to)
            {
                testObj->transform()->LookAtLerp(to, 0.5f);
            }
            testObj->transform()->position(
                from = Vec2::MoveTowards(
                    from,
                    to,
                    Input::GetKeyDown(KB_LSHIFT)      ? .2
                        : Input::GetKeyDown(KB_LCTRL) ? .0005f
                                                      : .05f));
            if(from == to)
            {
                if(paths.size() != arranged)
                    ++arranged;
                else
                    arranged = 0;
            }
        }
        else
        {
            // testObj->transform()->LookAt(p);
        }
    }
    move_targets(populations.size(), activeCount);

    player->transform()->position(Vec2::MoveTowards(player->transform()->position(), from, 0.085f));
}

void PathMover::OnGizmos()
{
    if(!showGizmosLayer)
        return;

    // draw nav path
    Gizmos::DrawNavMesh(population_navmesh);

    Color c(0, 200, 0);

    Gizmos::SetColor(c);

    if(!paths.empty())
    {
        auto iter = paths.cbegin();
        int j;

        Vec2 lp = population_navmesh->PointToWorldPoint(*begin(paths));
        for(j = 0; iter != _endIter && j <= arranged; ++iter, ++j)
        {
            if(lp != population_navmesh->PointToWorldPoint(*iter))
                Gizmos::DrawLine(lp, population_navmesh->PointToWorldPoint(*iter));
            lp = population_navmesh->PointToWorldPoint(*iter);
        }
        to = lp;

        c.b = 200;
        Gizmos::SetColor(c);
        for(; iter != _endIter; ++iter)
        {
            Gizmos::DrawLine(lp, population_navmesh->PointToWorldPoint(*iter));
            lp = population_navmesh->PointToWorldPoint(*iter);
        }
    }
}

void PathMover::OnUnloading()
{
    testObj = nullptr;
    to = Vec2::zero;
    last = nullptr;
    RoninMemory::free(population_navmesh);
    paths.clear();
    populations.clear();
    populations.shrink_to_fit();

    population_navmesh = nullptr;
}

void move_targets(int max, int &activeCount)
{
    static int y = 0;
    int x;
    if(max < 0 || max > populations.size())
        max = populations.size();

    if(y >= populations.size())
        y = 0;

    max = std::min<int>(max, populations.size() - y);

    for(x = 0; x < max; ++x)
    {
        auto *transform_wrap = std::get<Transform *>(populations[x + y]);
        float &time = std::get<float>(populations[x + y]);
        NavListSite &paths = std::get<std::remove_reference<decltype(paths)>::type>(populations[x + y]);

        if(time == 0)
            time = 1;

        if(!paths.empty())
        {
            if(transform_wrap->position() == population_navmesh->PointToWorldPoint(*paths.end()))
            {
            }
            else
            {
                Vec2 dir = population_navmesh->PointToWorldPoint(*paths.begin());
                if(transform_wrap->position() != dir)
                    transform_wrap->LookAtLerp(dir, 0.5f);
                transform_wrap->position(Vec2::MoveTowards(
                    transform_wrap->position(),
                    dir,
                    Input::GetKeyDown(KB_LSHIFT)      ? 2
                        : Input::GetKeyDown(KB_LCTRL) ? .005f
                                                      : .05f));
                if(transform_wrap->position() == dir)
                {
                    paths.erase(paths.begin());
                }
                ++activeCount;
            }
        }
        else
        {
            NavResultSite result;
            if(!population_navmesh->Find(
                   result,
                   population_navmesh->get(transform_wrap->position()),
                   population_navmesh->get(Random::Range(0, NavMeshMagnitude), Random::Range(0, NavMeshMagnitude))))
            {
                float a = transform_wrap->angle() + 1;
                if(a > 360)
                    a -= 360;
                transform_wrap->angle(a);
            }
            else
            {
                paths = std::move(result.connections);
            }
            population_navmesh->Clear();
        }
    }
    y += max;
}
