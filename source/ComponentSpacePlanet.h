#ifndef SPACEPLANET_H
#define SPACEPLANET_H

#include "SpaceMainMenu.h"

using namespace RoninEngine::Runtime;

class SpacePlanet;

class SpacePilot : public Behaviour
{
public:
    float speed = 2;
    Vec2 toWarn;
    SpacePlanet* owner_planet;

    SpacePilot() = default;

    void OnUpdate();
};

class SpaceWorker : public Behaviour
{
    float last_upload;

public:
    int maximumResources = 5;
    int resources;
    int switch_state;
    float speed = 1;
    float delay_load = 1;
    SpacePlanet* owner_planet;

    SpaceWorker() = default;

    void OnUpdate();
    void OnGizmos();
};

class SpaceResourcePlant : public Behaviour
{
public:
    int maxResources = 10000;
};

class SpacePlanet : public Behaviour
{
    std::list<float> damaged_lines;

public:
    SpaceResourcePlant* resource_plant;
    SpacePlanet* target;
    int startWith = 3;
    int health = 100;
    int resources = 10;
    float lastTimeFab = 0;
    Color playerColor;
    SpacePilot* referencePilot;

    SpacePlanet();
    void OnStart();
    void OnUpdate();
    void OnGizmos();

    void send_damage(SpacePilot* p);
};

#endif // SPACEPLANET_H
