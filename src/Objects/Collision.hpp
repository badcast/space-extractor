#ifndef _COLLISION_HPP_
#define _COLLISION_HPP_

#include <functional> // std::function

#include "SpaceDepends.hpp"

class Collision : public Behaviour
{
public:
    void OnAwake();
    void OnUpdate();
    void OnGizmos();

public:
    int targetLayer;
    Vec2 collideSize;
    std::function<bool(Collision *self, Collision *target)> onCollision;
    
    Collision() : Behaviour("Collision"), targetLayer(-1)
    {
    }

};

#endif
