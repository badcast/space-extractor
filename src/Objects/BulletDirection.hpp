#pragma once
#include "SpaceDepends.hpp"

class BulletDirection : public Behaviour
{
public:
    Vec2 dir;
    float speed;
    BulletDirection() : Behaviour("BulletDirection")
    {}

    void OnStart();
    void OnUpdate();
};
