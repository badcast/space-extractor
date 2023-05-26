#ifndef SPACEPLANET_H
#define SPACEPLANET_H

#include <ronin/framework.h>

using namespace RoninEngine::Runtime;

class SpacePlanet;

class SpacePilot : public Behaviour
{
public:
    Vec2 toWarn;
    SpacePlanet* owner_planet;

    SpacePilot() = default;

    void OnUpdate();
};

class SpacePlanet : public Behaviour
{
public:
    SpacePlanet* target;
    int startWith = 5;
    int health = 1000;
    int resources = 1000;
    float lastTimeFab = 0;
    SpacePilot* referencePilot;

    SpacePlanet();
    void OnStart();
    void OnUpdate();
    void OnGizmos();

    void damage(SpacePilot *p);
};

#endif // SPACEPLANET_H
