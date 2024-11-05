#include "WParticleEdtitor.hpp"

using namespace RoninEngine::Runtime;
using namespace RoninEngine::UI;

uid sliderDuration;
uid sliderParticleCount;
uid sliderParticleLifetime;
uid sliderParticleSpeed;
uid sliderParticleSize;
uid checkBoxParticleEmit;
uid checkBoxParticleCenter;
uid checkBoxParticleWorldState;
uid checkBoxParticleRotation;
uid buttonParticleReset;
uid buttonCameraBackclear;
uid buttonParticleclearReserved;

void WParticleEdtitor::OnStart()
{
    Primitive::CreateCamera2D();

    particle = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();
    particle->transform()->layer(GameLayers::ParticleClass);
    particle->setSource(Primitive::CreateSpriteRectangle());
    particle->interval = 0;
    particle->randomDirection = true;
    particle->rotate = true;
    particle->speed = 10;
    particle->startWith = 10;
    particle->worldSpace = true;
    particle->destroyAfter = false;
    particle->setInterpolates(1, 0.2, 0.4f);
    particle->setSource(globalAssets.gameSprites->GetSprite("explode-v1"));
    particle->setColors(Color::red, Color::yellow, Color::transparent);
    particle->setSizes(Vec2::one / 20, Vec2::one / 10);

    Resolution res = RoninSimulator::GetCurrentResolution();
    Rect pos = {res.width - 125, res.height - 500, 120, 30};

    sliderParticleCount = GetGUI()->PushSlider(particle->startWith, 1, 200, pos);

    pos.y += pos.h;
    sliderParticleLifetime = GetGUI()->PushSlider(particle->getDuration(), 0.1f, 10, pos);

    pos.y += pos.h;
    sliderParticleSpeed = GetGUI()->PushSlider(particle->speed, 0.1, 20, pos);

    pos.y += pos.h;
    buttonParticleReset = GetGUI()->PushButton("Reset", {pos.getXY(), {60, 30}});

    buttonCameraBackclear = GetGUI()->PushButton("Back clear", {pos.getXY() + Vec2 {60, 0}, {60, 30}});

    pos.y += pos.h;
    buttonParticleclearReserved = GetGUI()->PushButton("ClearReserve", {pos.getXY(), {60, 30}});

    pos.y += pos.h;
    sliderParticleSize = GetGUI()->PushSlider(0.1, 0, 0.5, pos);

    pos.y += pos.h;
    checkBoxParticleEmit = GetGUI()->PushCheckBox(true, "Emit", pos);

    pos.y += pos.h;
    checkBoxParticleCenter = GetGUI()->PushCheckBox(true, "Center position", pos);
    pos.y += pos.h;
    checkBoxParticleWorldState = GetGUI()->PushCheckBox(true, "World State", pos);
    pos.y += pos.h;
    checkBoxParticleRotation = GetGUI()->PushCheckBox(true, "Rotate", pos);

    // GetGUI()->LayoutNew(UILayoutDirection::Horizontal, {0, 0, 100, 500});
    // GetGUI()->LayoutLabel("Text");
    // GetGUI()->LayoutButton("Hello ");
}

void WParticleEdtitor::OnUpdate()
{
    Vec2 point = GetGUI()->CheckBoxGetValue(checkBoxParticleCenter) ? Vec2::zero : Camera::ScreenToWorldPoint(Input::GetMousePointf());
    particle->transform()->position(point);

    if(GetGUI()->ButtonClicked(buttonParticleReset))
        particle->Reset();

    if(GetGUI()->ButtonClicked(buttonCameraBackclear))
        Camera2D::mainCamera()->backclear = !Camera2D::mainCamera()->backclear;

    if(GetGUI()->ButtonClicked(buttonParticleclearReserved))
        particle->ClearReserved();

    particle->emit = GetGUI()->CheckBoxGetValue(checkBoxParticleEmit);
    particle->startWith = static_cast<int>(GetGUI()->SliderGetValue(sliderParticleCount));
    particle->setInterpolates(GetGUI()->SliderGetValue(sliderParticleLifetime));
    particle->speed = GetGUI()->SliderGetValue(sliderParticleSpeed);
    particle->setSize(GetGUI()->SliderGetValue(sliderParticleSize) * Vec2::one);
    particle->worldSpace = GetGUI()->CheckBoxGetValue(checkBoxParticleWorldState);
    particle->rotate = GetGUI()->CheckBoxGetValue(checkBoxParticleRotation);
}
