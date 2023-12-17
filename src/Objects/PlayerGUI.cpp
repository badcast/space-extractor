#include "Player.hpp"

void Player::InitPlayerGUI()
{
}

void Player::OnGizmos()
{
    constexpr float size = 0.3f;
    static Vec2 v[3] {{size, size}, {size * 2, size}, {size * 1.5, size * 2}}, cv[3];
    static float angle = 0;

    // rotate
    for(int x = 0; x < 3; ++x)
    {
        Vec2 p = Vec2::RotateAround(gunPosition->position() + v[x], Vec2::up, angle * Math::deg2rad);
        Vec2 n = Vec2::RotateAround(gunPosition->position() + (x == 2 ? v[0] : v[x + 1]), Vec2::up, angle * Math::deg2rad);
        Gizmos::DrawLine(Vec2::Rotate(p, angle * Math::deg2rad), Vec2::Rotate(n, angle * Math::deg2rad));
    }
    angle += 5;

    Gizmos::DrawTextLegacy(Camera::ScreenToWorldPoint(Vec2::zero), "HEALTH: " + std::to_string(healthPoint));

    Gizmos::DrawTextLegacy(Camera::ScreenToWorldPoint(Vec2{0,18}), "SCORES: " + std::to_string(scores));
}
