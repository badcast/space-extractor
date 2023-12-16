#include "SpaceMainMenu.h"

using namespace RoninEngine;

void OverlayLevel::OnStart()
{
    switch_game_level(this);
    Primitive::CreateEmptyGameObject()->AddComponent<Camera2D>();

    bodySize = { 1, 1.f / 2 };
    bublePos = Camera::ViewportToWorldPoint(Vec2::zero) + Vec2(bodySize.x, bodySize.y * -1);
    targetPos = bublePos;
    trigger = 0;
    cursor, plane;
    speed = 5;
}

Vec2 getRandomPoint() { return Camera::ViewportToWorldPoint({ Random::Value(), Random::Value() }); }

Vec2 getRandomPointOutSide(const Vec2& bodySize)
{
    Vec2 alpha = Camera::ViewportToWorldPoint(Vec2::zero);
    Vec2 beta = Camera::ViewportToWorldPoint(Vec2::one);
    Vec2 pt = getRandomPoint();
    pt.x = Math::Outside(pt.x, alpha.x + bodySize.x / 2, beta.x - bodySize.x / 2);
    pt.y = Math::Outside(pt.y, alpha.y - bodySize.y / 2, beta.y + bodySize.y / 2);
    return pt;
}

void OverlayLevel::OnGizmos()
{
    if (trigger % 3 == 0) {
        Gizmos::SetColor(Color::gray);
        plane = bublePos;
    } else if (Vec2::Distance(cursor, plane) < 0.1f) {
        plane = getRandomPoint();
    }

    cursor = Vec2::MoveTowards(cursor, plane, TimeEngine::deltaTime() * speed);

    bool inter = Vec2::HasIntersection({ cursor, bodySize }, { bublePos, bodySize });
    if (!inter)
        Gizmos::SetColor(Color::gray);
    else
        cursor = getRandomPoint() * 2;
    if (bublePos == targetPos || inter) {
        targetPos = getRandomPointOutSide(bodySize);
        ++trigger;
    }

    Gizmos::DrawRectangleRounded(cursor, 0.5, 0.5, 25);

    Gizmos::DrawFillRect(bublePos, bodySize.x, bodySize.y);
    Gizmos::SetColor(Color::black);
    Gizmos::DrawText(bublePos, std::to_string(trigger));
    Gizmos::SetColor(Color::red);
    Gizmos::DrawPosition(plane, 0.1);

    bublePos = Vec2::MoveTowards(bublePos, targetPos, TimeEngine::deltaTime() * speed);
}
