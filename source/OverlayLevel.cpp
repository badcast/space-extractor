#include "gamemainmenu.h"

#include "OverlayLevel.h"

using namespace RoninEngine;
#include <iostream>

void OverlayLevel::on_awake() { }

void OverlayLevel::on_start()
{
    switch_game_level(this);
    create_game_object()->add_component<Camera2D>();
}

void OverlayLevel::on_update() { }

void OverlayLevel::on_late_update() { }

Vec2 getRandomPoint() { return Camera::viewport_2_world({ Random::value(), Random::value() }); }

Vec2 getRandomPointOutSide(const Vec2& bodySize)
{
    Vec2 alpha = Camera::viewport_2_world(Vec2::zero);
    Vec2 beta = Camera::viewport_2_world(Vec2::one);
    Vec2 pt = getRandomPoint();
    pt.x = Math::outside(pt.x, alpha.x + bodySize.x / 2, beta.x - bodySize.x / 2);
    pt.y = Math::outside(pt.y, alpha.y - bodySize.y / 2, beta.y + bodySize.y / 2);
    return pt;
}

void OverlayLevel::on_gizmo()
{
    // Gizmos::setColor(Color::red);
    static Vec2 bodySize = { 1, 1.f / 2 };
    static Vec2 bublePos = Camera::viewport_2_world(Vec2::zero) + Vec2(bodySize.x, bodySize.y * -1);
    static Vec2 targetPos = bublePos;
    static int trigger = 0;
    static Vec2 cursor, plane;
    static float speed = 5;

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

void OverlayLevel::on_unloading() { }
