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
    SpriteRef defaultTurret, fireTurret;

    void onRepairing();

public:
    Weapon *weapon;
    TransformRef platform;
    TransformRef turretRotatePivot;
    TransformRef turret;
    TransformRef gunPosition;
    TransformRef muzzleFlash;
    TransformRef gunPoint1, gunPoint2;
    TransformRef playerShield;
    AudioSourceRef playerAudio;

    bool canRepairs = true;
    float repairsAfterTime = 1.0f;
    int repairScore = 1;

    int maxHealthPoint = 100;
    int healthPoint = 0;
    int maxArmoryPoint = 50;
    int armoryPoint = 0;
    int scores = 0;

    bool canClampAngle = true;
    float clampAngleLeft = -90, clampAngleRight = 90;

    void applyDamage(int damageValue, Vec2 closestPosition);

public:
    void OnStart();
    void OnUpdate();
    void OnGizmos();
    void OnDestroy();
};

#endif
