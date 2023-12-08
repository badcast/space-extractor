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
    void OnStart();
    void OnUpdate();
    void OnGizmos();
    void OnDestroy();

private:
    void InitPlayerGUI();
    void showShield(Vec2 lookPosition);

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
    int maxHealthPoint = 1000;
    int healthPoint = 0;
    int maxArmoryPoint = 100;
    int armoryPoint = 0;
    int scores = 0;
    float clampAngleLeft = -80, clampAngleRight = 80;

    std::set<Transform *> bullets;

    void applyDamage(int damageValue, Vec2 closestPosition);
};

#endif
