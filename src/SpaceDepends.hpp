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
class Particle;

enum Layers : int
{
    All = -1,
    ParticleClass = 5,
    BulletClass = 1,
    ShieldClass = 2,
    EnemyClass = 4,
    PlayerClass = 8,
    EnemyOrBullet = Layers::EnemyClass | Layers::BulletClass,
    PlayerOrBullet = Layers::PlayerClass | Layers::BulletClass,
};

#include "Objects/EnemyClass.hpp"
#include "Objects/Weapon.hpp"
#include "Objects/WeaponMachineGun.hpp"
#include "Objects/Collision.hpp"
#include "Objects/Particle.hpp"

#endif
