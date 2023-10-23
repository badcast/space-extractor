#include "WParticleEdtitor.hpp"

using namespace RoninEngine::UI;

void WParticleEdtitor::OnStart()
{
    Primitive::CreateCamera2D();

    particle = Primitive::CreateEmptyGameObject()->AddComponent<Particle>();
    particle->source = Primitive::CreateSpriteCircle();
    particle->interval = 0.05f;
    particle->randomDirection = true;
    particle->rotate = true;
    particle->speed = 10;
    particle->startWith = 3;
    particle->setInterpolates(4, 0.2, 0.4f);
    particle->setColors(Color::transparent, Color::yellow, Color::transparent);
    particle->setSizes(Vec2::one / 10, Vec2::one);
    particle->worldSpace = true;
}

uid sliderDuration;

void WParticleEdtitor::OnUpdate()
{
    //Vec2 point = Camera::ScreenToWorldPoint(Input::GetMousePointf());
    //particle->transform()->position(point);

}
