/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _WPARTICLE_EDITOR_HPP_
#define _WPARTICLE_EDITOR_HPP_

#include "SpaceExtractor.hpp"

class WParticleEdtitor : public World
{
public:
    ParticleSystem* particle;
    WParticleEdtitor() : World("Particle Editor")
    {
    }

    void OnStart();
    void OnUpdate();
};


class TestLevel : public World
{
public:
    Transform *root;
    Transform *last;

    std::vector<Transform *> list;

    int parentN = 10;
    TestLevel() : World("Empty World")
    {
    }

    void OnAwake()
    {
        root = Primitive::CreateBox2D()->transform();

        last = root;
        for(int x = 0; x < parentN; ++x)
        {
            Transform *b = Primitive::CreateBox2D()->transform();
            b->transform()->position(last->position() + Vec2::up + Random::RandomVector());
            b->spriteRenderer()->setSize({.5f, 1.0f});
            b->spriteRenderer()->setColor(Color::GetBasicColors()[x % Color::GetBasicColors().size()]);

            b->transform()->setParent(last);

            last = b;
        }

        list = root->GetAllTransforms();

        Primitive::CreateCamera2D();
    }

    void OnUpdate()
    {
        Vec2 mousePosition = Camera::ScreenToWorldPoint(Input::GetMousePointf());

        if(Input::GetMouseDown(MouseButton::MouseLeft))
            root->position(mousePosition);

        if(Input::GetMouseDown(MouseButton::MouseRight))
            last->position(mousePosition);

        root->angle(root->angle() + Input::GetMouseWheel() * 4);

        Camera::mainCamera()->transform()->Translate(Input::GetAxis() * .1f);
    }

    void OnGizmos()
    {
        Vec2 point = last->position();

        RenderUtility::DrawTextLegacy(point, std::to_string(point.x) + std::string(" ") + std::to_string(point.y), false, true);
    }
};


#endif
