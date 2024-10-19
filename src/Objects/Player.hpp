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
private:
    struct
    {
        int force;
        float time;
    } shaking;

    void InitPlayerGUI();
    void showShield(Vec2 lookPosition);

protected:
    float m_timeoutRepairs;

    void onRepairing();

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

    bool canRepairs = true;
    float repairsAfterTime = 1.0f;
    int repairScore = 1;

    int maxHealthPoint = 100;
    int healthPoint = 0;
    int maxArmoryPoint = 50;
    int armoryPoint = 0;
    int scores = 0;
    float clampAngleLeft = -80, clampAngleRight = 80;

    std::set<Transform *> bullets;

    void applyDamage(int damageValue, Vec2 closestPosition);

public:
    void OnStart();
    void OnUpdate();
    void OnGizmos();
    void OnDestroy();
};

#endif
