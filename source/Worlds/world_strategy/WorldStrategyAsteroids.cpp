#include <fstream>

#include "../../SpaceMainMenu.h"
#include "ComponentSpacePlanet.h"

uid but;
uid textGameTime;
uid butSave, butLoad;
const char *fsave = "./pointer.dat";
void callback(uid id, void *userData)
{
    if(id == but)
    {
        Camera::mainCamera()->transform()->position(Vec2::zero);
    }
    else if(id == butSave)
    {
        Vec2 p = Camera::mainCamera()->transform()->position();
        std::fstream f {fsave, std::ios_base::out};
        if(!f)
        {
            RoninSimulator::ShowMessage("failed saving file");
            return;
        }
        f.write((char *) &p, sizeof p);

        f.close();
    }
    else if(id == butLoad)
    {
        Vec2 p;
        std::fstream f {fsave, std::ios_base::in};
        if(!f)
        {
            RoninSimulator::ShowMessage("failed load file");
            return;
        }
        f.read((char *) &p, sizeof p);

        f.close();

        Camera::mainCamera()->transform()->position(p);
    }
}

void SpaceExtractorLevel::OnStart()
{
    switch_game_level(this);

    auto camera = Primitive::create_camera2D(Vec2::zero);
    camera->visibleGrids = false;
    camera->visibleBorders = true;
    camera->visibleNames = false;
    camera->gameObject()->AddComponent<MoveController2D>();

    GameObject *referencePlanet = Primitive::create_empty_game_object(Vec2::up * 2);
    referencePlanet->AddComponent<SpacePlanet>();

    SpriteRenderer *lastSpr = referencePlanet->AddComponent<SpriteRenderer>();
    lastSpr->size = Vec2::one / 5;
    lastSpr->set_sprite(Primitive::create_sprite2D_circle(Vec2::one, 1, Color::red));
    lastSpr = referencePlanet->AddComponent<SpriteRenderer>();
    lastSpr->size = Vec2::one / 2;
    lastSpr->set_sprite(Primitive::create_sprite2D_circle(Vec2::one, 1, Color::darkgray));

    GameObject *referencePlanet2 = Instantiate(referencePlanet, Vec2::down * 2);
    referencePlanet2->AddComponent<SpacePlanet>();
    // UI
    getGUI()->SetGeneralCallback(callback, nullptr);
    but = getGUI()->PushButton("center camera", {700, 0, 90, 20});
    butSave = getGUI()->PushButton("save", {792, 0, 80, 20});
    butLoad = getGUI()->PushButton("load", {874, 0, 80, 20});
    textGameTime = getGUI()->PushLabel("", Vec2Int(0, (RoninSimulator::GetCurrentResolution()).height - 17));

    stars[0] = Primitive::create_empty_game_object()->transform();
    lastSpr = stars[0]->gameObject()->AddComponent<SpriteRenderer>();
    lastSpr->set_sprite(Primitive::create_sprite2D_triangle());
    lastSpr->size /= 32;
    stars[0]->angle(90);
    for(int x = 1; x < stars.size(); ++x)
    {
        stars[x] = Instantiate(stars[0]->gameObject())->transform();
        stars[x]->position(Camera::ViewportToWorldPoint(Random::RandomVector()));
    }
}

void SpaceExtractorLevel::OnUpdate()
{
    for(int x = 0; x < stars.size(); ++x)
    {
        Transform *t = stars[x];
        Vec2 v = t->position();
        if(Math::Round(t->position().x) <= Math::Round(dmgReg.x))
        {
            v.x = -dmgReg.x;
            t->position(v);
            continue;
        }
        t->position(Vec2::MoveTowards(v, {dmgReg.x, v.y}, TimeEngine::deltaTime() * 5));
    }
}

void SpaceExtractorLevel::OnGizmos()
{
}

void SpaceExtractorLevel::OnUnloading()
{
}
