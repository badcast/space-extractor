/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _WEAPON_MACHINE_GUN_HPP_
#define _WEAPON_MACHINE_GUN_HPP_

#include "Weapon.hpp"

class WeaponMachineGun : public Weapon
{
public:
    WeaponMachineGun()
    {
        setDefaultValues();
    }

    void setDefaultValues() override;

    GameObjectRef createBullet() override;
};

#endif
