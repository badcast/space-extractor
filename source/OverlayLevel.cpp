#include "SpaceMainMenu.h"

using namespace RoninEngine;

void OverlayLevel::on_start()
{
    switch_game_level(this);
    create_game_object()->add_component<Camera2D>();

    bodySize = { 1, 1.f / 2 };
    bublePos = Camera::viewport_to_world(Vec2::zero) + Vec2(bodySize.x, bodySize.y * -1);
    targetPos = bublePos;
    trigger = 0;
    cursor, plane;
    speed = 5;
}

Vec2 getRandomPoint() { return Camera::viewport_to_world({ Random::value(), Random::value() }); }

Vec2 getRandomPointOutSide(const Vec2& bodySize)
{
    Vec2 alpha = Camera::viewport_to_world(Vec2::zero);
    Vec2 beta = Camera::viewport_to_world(Vec2::one);
    Vec2 pt = getRandomPoint();
    pt.x = Math::outside(pt.x, alpha.x + bodySize.x / 2, beta.x - bodySize.x / 2);
    pt.y = Math::outside(pt.y, alpha.y - bodySize.y / 2, beta.y + bodySize.y / 2);
    return pt;
}

void OverlayLevel::on_gizmo()
{
    if (trigger % 3 == 0) {
        Gizmos::set_color(Color::gray);
        plane = bublePos;
    } else if (Vec2::distance(cursor, plane) < 0.1f) {
        plane = getRandomPoint();
    }

    cursor = Vec2::move_towards(cursor, plane, TimeEngine::deltaTime() * speed);

    bool inter = Vec2::has_intersection({ cursor, bodySize }, { bublePos, bodySize });
    if (!inter)
        Gizmos::set_color(Color::gray);
    else
        cursor = getRandomPoint() * 2;
    if (bublePos == targetPos || inter) {
        targetPos = getRandomPointOutSide(bodySize);
        ++trigger;
    }

    Gizmos::draw_rectangle_rounded(cursor, 0.5, 0.5, 25);

    Gizmos::draw_fill_rect(bublePos, bodySize.x, bodySize.y);
    Gizmos::set_color(Color::black);
    Gizmos::draw_text(bublePos, std::to_string(trigger));
    Gizmos::set_color(Color::red);
    Gizmos::draw_position(plane, 0.1);

    bublePos = Vec2::move_towards(bublePos, targetPos, TimeEngine::deltaTime() * speed);
}
