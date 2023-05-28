#include "gamelevel.h"
#include <fstream>

#include "space_planet.h"

uid but;
uid textGameTime;
uid butSave, butLoad;
const char* fsave = "./pointer.dat";
void callback(uid id, void* userData)
{
    if (id == but) {
        Camera::main_camera()->transform()->position(Vec2::zero);
    } else if (id == butSave) {
        Vec2 p = Camera::main_camera()->transform()->position();
        std::fstream f { fsave, std::ios_base::out };
        if (!f) {
            Application::show_message("failed saving file");
            return;
        }
        f.write((char*)&p.x, sizeof p.x);
        f.write((char*)&p.y, sizeof p.y);
        f.close();
    } else if (id == butLoad) {
        Vec2 p;
        std::fstream f { fsave, std::ios_base::in };
        if (!f) {
            Application::show_message("failed load file");
            return;
        }
        f.read((char*)&p.x, sizeof p.x);
        f.read((char*)&p.y, sizeof p.y);
        f.close();

        Camera::main_camera()->transform()->position(p);
    }
}

std::vector<Transform*> stars(100, nullptr);
Runtime::Vec2 dmgReg { -6, 6 };
void SpaceExtractorLevel::on_start()
{
    auto camera = Primitive::create_camera2D(Vec2::zero);
    camera->visibleGrids = false;
    camera->visibleBorders = false;
    camera->visibleNames = false;
    camera->game_object()->add_component<MoveController2D>();

    GameObject* referencePlanet = Primitive::create_empty_game_object(Vec2::up * 2);
    referencePlanet->add_component<SpacePlanet>();

    SpriteRenderer* lastSpr = referencePlanet->add_component<SpriteRenderer>();
    lastSpr->size = Vec2::one / 5;
    lastSpr->set_sprite(Primitive::create_sprite2D_circle(Vec2::one, 1, Color::red));
    lastSpr = referencePlanet->add_component<SpriteRenderer>();
    lastSpr->size = Vec2::one / 2;
    lastSpr->set_sprite(Primitive::create_sprite2D_circle(Vec2::one, 1, Color::darkgray));

    GameObject* referencePlanet2 = instantiate(referencePlanet, Vec2::down * 2);
    referencePlanet2->add_component<SpacePlanet>();
    // UI
    ui->register_callback(callback, nullptr);
    but = ui->push_button("center camera", { 700, 0, 90, 20 });
    butSave = ui->push_button("save", { 792, 0, 80, 20 });
    butLoad = ui->push_button("load", { 874, 0, 80, 20 });
    textGameTime = ui->push_label("", Vec2Int(0, (Application::get_resolution()).height - 17));

    stars[0] = Primitive::create_empty_game_object()->transform();
    lastSpr = stars[0]->game_object()->add_component<SpriteRenderer>();
    lastSpr->set_sprite(Primitive::create_sprite2D_triangle());
    lastSpr->size /= 32;
    stars[0]->angle(90);
    for (int x = 1; x < stars.size(); ++x) {
        stars[x] = instantiate(stars[0]->game_object())->transform();
        stars[x]->position(Camera::viewport_2_world(Random::randomVector()));
    }
}

void SpaceExtractorLevel::on_update()
{
    int time = TimeEngine::time();
    ui->set_text(textGameTime, "Game time: " + std::string((time / 60 < 10) ? "0" : "") + std::to_string(time / 60) + std::string(":") + std::string((time % 60 < 10) ? "0" : "") + std::to_string(time % 60));

    for (int x = 0; x < stars.size(); ++x) {
        Transform* t = stars[x];
        Vec2 v = t->position();
        if (t->position().x <= dmgReg.x) {
            v.x = -dmgReg.x;
            t->position(v);
            continue;
        }
        t->position(Vec2::move_towards(v, { dmgReg.x, v.y }, TimeEngine::deltaTime() * 4));
    }
}

void SpaceExtractorLevel::on_gizmo() {  }

void SpaceExtractorLevel::on_unloading() { }
