#include <fstream>
#include "SpaceMainMenu.h"

constexpr char text_path_found[] = "Path found!";
constexpr char text_path_not_found[] = "Path not found!";

using namespace RoninEngine::Runtime;
struct
{
    Color defaultNeuron = Color(0xfff6f723);
    Color defaultBackground = Color(0xff1e1e1e);
    Color deafultFore = Color(0xff90791C);
    Color line = Color::red;
    Color pointStart = Color::red;

    AI::NavResultSite result;
    Vec2 first;
    Vec2 last;
    int animate_per_block = 10;
    int animate_positon;

    const char *animate_text;
    Vec2 animate_text_start_pos;
    float animate_text_lasttime;

    Vec2 cellSize = Vec2(0.3, 0.3);
    float cellThickness = 0.001f;
} setting;
NavMesh *navMesh;
uid but_ch;
uid but_sv;
uid but_ld;
uid but_mz;
uid but_cl;
uid sldr_zoom;

void refind()
{
    navMesh->Clear();
    navMesh->Find(setting.result, setting.first, setting.last);
    setting.animate_text = setting.result.status == AI::NavStatus::Opened ? text_path_found : text_path_not_found;
    Resolution resolution = RoninSimulator::GetCurrentResolution();
    setting.animate_text_start_pos =
        Camera::ScreenToWorldPoint({static_cast<float>(resolution.width), static_cast<float>(resolution.height - 20)});
    setting.animate_text_lasttime = TimeEngine::time() + 2;
}

void world_load_save(uid but)
{
    constexpr auto ff = "./nvmsh";
    NavMeshData nmsh;
    if(but == but_ch)
    {
        NavIdentity identity = navMesh->getIdentity();
        navMesh->setIdentity(identity == NavIdentity::PlusMethod ? NavIdentity::DiagonalMethod : NavIdentity::PlusMethod);
    }
    else if(but == but_sv)
    {
        std::ofstream ofs {ff};
        if(!ofs)
        {
            RoninSimulator::ShowMessage("fail saving");
            return;
        }
        navMesh->SaveTo(&nmsh);
        ofs.write((char *) &nmsh.widthSpace, sizeof(nmsh.widthSpace));
        ofs.write((char *) &nmsh.heightSpace, sizeof(nmsh.heightSpace));
        ofs.write((char *) nmsh.data, nmsh.len);
        std::free(nmsh.data);
        ofs.close();
    }
    else if(but == but_ld)
    {
        std::ifstream ifs {ff};
        std::ifstream::pos_type off;
        if(!ifs)
        {
            RoninSimulator::ShowMessage("fail load");
            return;
        }
        ifs.read((char *) &nmsh.widthSpace, sizeof(nmsh.widthSpace));
        ifs.read((char *) &nmsh.heightSpace, sizeof(nmsh.heightSpace));
        off = nmsh.len = ifs.tellg();
        nmsh.len = ifs.seekg(0, std::ios_base::end).tellg() - off;
        ifs.seekg(off);
        nmsh.data = std::malloc(nmsh.len);
        ifs.read((char *) nmsh.data, nmsh.len);

        navMesh->LoadFrom(nmsh);
        std::free(nmsh.data);
        ifs.close();
    }
    else if(but == but_mz)
    {
        navMesh->GenerateMaze(MazeAlgorithm::ForcedChaos);
    }
    else if(but == but_cl)
    {
        navMesh->Clear(true);
    }
    setting.result.connections.clear();
    refind();
}

void zoom_changed(uid, float newValue)
{
    setting.cellSize = Vec2::one * newValue;
    navMesh->worldScale = setting.cellSize;
}

void SectorLine::OnStart()
{
    ::navMesh = &this->navMesh;
    switch_game_level(this);
    Primitive::CreateCamera2D()->AddComponent<MoveController2D>();
    this->navMesh.worldScale = Vec2::Scale(navMesh.worldScale, setting.cellSize);

    Rect r {400, 0, 80, 16};
    GetGUI()->PushLabel("zoom", r);
    r.x += r.w;
    sldr_zoom = GetGUI()->PushSlider(setting.cellSize.x, 0.05f, 10, {r.GetXY(), {80, 16}}, zoom_changed);
    r.x += r.w + 10;
    but_ch = GetGUI()->PushButton("method", r, world_load_save);
    r.x += r.w;
    but_sv = GetGUI()->PushButton("save", r, world_load_save);
    r.x += r.w;
    but_ld = GetGUI()->PushButton("load", r, world_load_save);
    r.x += r.w;
    but_mz = GetGUI()->PushButton("maze", r, world_load_save);
    r.x += r.w;
    but_cl = GetGUI()->PushButton("clear", r, world_load_save);
    // generate maze
    navMesh.GenerateMaze();
    setting.result.connections.clear();
    setting.first = setting.last = Vec2::zero;
}

void SectorLine::OnGizmos()
{
    Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());
    if(GetGUI()->IsMouseOver() == false)
    {
        if(Input::GetMouseDown(MouseButton::MouseLeft))
            setting.first = ms;
        else if(Input::GetMouseDown(MouseButton::MouseRight))
            setting.last = ms;
        else
        {
            static bool dr = true;
            static bool fc = false;
            if(Input::GetMouseDown(MouseButton::MouseMiddle))
            {
                auto p = navMesh.WorldPointToPoint(ms);
                if(fc == 0)
                {
                    dr = !navMesh.hasLocked(p);
                    fc = true;
                }
                navMesh.setLock(p, dr);
            }
            else if(Input::GetMouseUp(MouseButton::MouseMiddle))
            {
                fc = false;
            }

            if(Input::GetMouseUp(MouseButton::MouseLeft) || Input::GetMouseUp(MouseButton::MouseRight))
            {
                refind();
            }
        }
    }

    auto neuron = navMesh.get(setting.first);
    if(neuron)
        neuron->h = 1;

    // Draw nav mesh
    Gizmos::DrawNavMesh(&navMesh);

    if(setting.result.status == AI::NavStatus::Opened)
    {
        Vec2 first = navMesh.PointToWorldPoint(setting.result.from);
        Gizmos::SetColor(setting.pointStart);
        int x = 0;
        int y = setting.animate_positon;
        for(; x < y; ++x)
        {
            auto n = setting.result.connections[x];
            Gizmos::DrawLine(first, navMesh.PointToWorldPoint(n));
            first = navMesh.PointToWorldPoint(n);
        }

        setting.animate_positon = Math::Min<int>(setting.animate_positon + setting.animate_per_block, setting.result.connections.size());
    }

    // show animate text
    if(setting.animate_text)
    {
        Resolution resolution = RoninSimulator::GetCurrentResolution();
        Vec2 to = Camera::ScreenToWorldPoint({static_cast<float>(resolution.width - 150), static_cast<float>(resolution.height - 20)});
        Vec2 diff; // setting.animate_text_start_pos;

        setting.animate_text_start_pos = Vec2::MoveTowards(setting.animate_text_start_pos, to, TimeEngine::deltaTime() * 7);

        Gizmos::DrawTextLegacy(setting.animate_text_start_pos, setting.animate_text);
        if(TimeEngine::time() > setting.animate_text_lasttime)
        {
            setting.animate_text = nullptr;
        }
    }

    Gizmos::SetColor(Color::blue);
    Gizmos::DrawFillCircle(setting.first, 0.3 * navMesh.worldScale.x);
    Gizmos::SetColor(Color::green);
    Gizmos::DrawFillCircle(setting.last, 0.3 * navMesh.worldScale.x);
}
