/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _SPACE_DEPENDS_HPP_
#define _SPACE_DEPENDS_HPP_

#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AI;

class Player;
class Enemy;
class Weapon;
class Collision;

enum GameLayers : int
{
    All = -1,
    AnyClass = 0,
    BulletClass = 1,
    ShieldClass = 2,
    EnemyClass = 4,
    ParticleClass = 6,
    PlayerClass = 8,
    EnemyOrBullet = GameLayers::EnemyClass | GameLayers::BulletClass,
    PlayerOrBullet = GameLayers::PlayerClass | GameLayers::BulletClass,
};

enum RenderOrders : int
{
    BackgroundOrder = -100,
    OtherOrder = 0,
    PlayerOrder,
    EnemyOrder,
    ParticlesOrder,
    ParticleMainSmokeOrder
};

#include "GameAssets.hpp"

#include "Objects/EnemyClass.hpp"
#include "Objects/SpriteAnimator.hpp"
#include "Objects/Weapon.hpp"
#include "Objects/WeaponMachineGun.hpp"
#include "Objects/Collision.hpp"

#include "PlayerCursor.hpp"

#endif
