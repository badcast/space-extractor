
#include "BunkerGame.h"

using namespace RoninEngine::Runtime;

void BunkerWorld::on_start()
{
    switch_game_level(this);

    Primitive::create_camera2D();

    turret = Primitive::create_empty_game_object();

    turret->add_component<SpriteRenderer>()->set_sprite(Primitive::create_sprite2D_triangle(Vec2::half / 2, 1, Color::red));
}

void BunkerWorld::on_update()
{
    Vec2 ms { Input::get_mouse_point() };

    turret->transform()->look_at_lerp(Camera::screen_to_world(ms), TimeEngine::deltaTime());
}

void BunkerWorld::on_gizmo()
{
    Vec2 ms = Camera::screen_to_world(Input::get_mouse_pointf());
    Gizmos::draw_position(ms, 0.1f);
    Gizmos::draw_circle(ms, 0.1f);

    Vec2 forward = turret->transform()->position();
    forward = Vec2::rotate(forward + Vec2::up, turret->transform()->angle());
    Gizmos::draw_line(forward, forward *2);
}
