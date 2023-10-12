/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "SpaceExtractor.hpp"

class Player : public Behaviour
{
public:
    Weapon *weapon;
    Transform *platform;
    Transform *turretRotatePivot;
    Transform *turret;
    Transform *gunPosition;
    Transform *muzzleFlash;
    Transform *gunPoint1, *gunPoint2;
    Transform *playerShield;
    AudioSource *playerAudio;
    int healthPoint = 1000;
    float clampAngleLeft = -80, clampAngleRight = 80;

    std::set<Transform*> bullets;

    void OnStart();
    void OnUpdate();
    void OnGizmos();
    void OnDestroy();

    void applyDamage(int damage);
};

#endif
