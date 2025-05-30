/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _WEAPON_BASE_HPP_
#define _WEAPON_BASE_HPP_

#include "SpaceDepends.hpp"

class Weapon
{
public:
    float rotateSpeed;
    float bulletSpeed;
    float bulletDelayShot;
    float bulletThreshold;
    float weight;
    int damage;
    std::vector<AudioClip*> clips;

    Weapon() : rotateSpeed(0), bulletSpeed(0), bulletDelayShot(0), bulletThreshold(0), weight(0), damage(0)
    {
    }

    virtual void setDefaultValues() = 0;
    virtual GameObjectRef createBullet() = 0;
};

#endif
