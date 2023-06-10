
#include "SpaceMainMenu.h"

using namespace RoninEngine::Runtime;

constexpr float bullet_speed = 10.f;
constexpr float bullet_delay_shot = 0.08f;
constexpr float bullet_destroy_after = 5; // seconds
constexpr float turret_rotate_speed = 4.f;

uid bVSYN;
void BunkerWorld::on_start()
{
    switch_game_level(this);
    Camera2D* camera = Primitive::create_camera2D();
    camera->game_object()->add_component<MoveController2D>();

    //    camera->visibleGrids = true;

    constexpr int max_tur = 8;
    for (int x = 0; x < max_tur; ++x) {
        GameObject* turret = Primitive::create_empty_game_object();
        turret->add_component<SpriteRenderer>()->set_sprite(Primitive::create_sprite2D_triangle(Vec2::half / 2, 1, Color::red));
        turret->transform()->position(Camera::viewport_to_world(Vec2::one * Random::value())); // set position to Turret
        turrets.push_back(turret->transform());
    }

    bulTempl = Primitive::create_empty_game_object();
    bulTempl->add_component<SpriteRenderer>()->set_sprite(Primitive::create_sprite2D_box({ 0.02f, 0.09f }, Color::red));
    bulTempl->set_active(false);
}

void BunkerWorld::on_update()
{
    Vec2 ms { Input::get_mouse_point() };
    for (Transform* turret : turrets) {
        turret->look_at_lerp(Camera::screen_to_world(ms), TimeEngine::deltaTime() * turret_rotate_speed);
    }
    // remove destroyed from list
    bullets.remove_if([](Transform* each) { return !instanced(each); });

    for (Transform* bul : bullets) {
        Vec2 bulPos = bul->position();
        bulPos = Vec2::move_towards(bulPos, bul->forward() * 10, TimeEngine::deltaTime() * bullet_speed);
        bul->position(bulPos);
    }

    if (last_time < TimeEngine::time() && Input::is_mouse_down()) {
        last_time = bullet_delay_shot + TimeEngine::time();
        for (Transform* turret : turrets) {
            Vec2 origin = turret->transform()->position();
            Vec2 dir = turret->transform()->right();
            dir += Random::random_vector() / 35;
            GameObject* bul = instantiate(bulTempl, origin);
            bul->transform()->look_at(dir * 100);
            bullets.emplace_back(bul->transform());
            bul->destroy(bullet_destroy_after);
        }
    }
}

void BunkerWorld::on_gizmo()
{
    Vec2 ms = Camera::screen_to_world(Input::get_mouse_pointf());
    Gizmos::draw_position(ms, 0.1f);
    Gizmos::draw_circle(ms, 0.1f);
}
