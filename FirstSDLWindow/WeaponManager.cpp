#include "WeaponManager.h"
#include <SDL.h>
#include <stdio.h>

WeaponManager::WeaponManager()
    : currentWeapon(WeaponType::DEFAULT), currentAmmo(-1)
{

}

void WeaponManager::setWeapon(WeaponType type, int ammo) 
{
    currentWeapon = type;
    currentAmmo = ammo;
}

void WeaponManager::update() 
{
    // Decrease ammo if needed or handle cooldowns
    if (currentWeapon != WeaponType::DEFAULT && currentAmmo == 0) 
    {
        setWeapon(WeaponType::DEFAULT);
    }
}

bool WeaponManager::canShoot() const 
{
    return currentAmmo != 0;
}

WeaponType WeaponManager::getCurrentWeapon() const 
{
    return currentWeapon;
}

void WeaponManager::onShoot() {
    if (currentWeapon == WeaponType::PIERCING_SHOT) 
    {
        // Trigger piercing shot logic
        printf("Sniper Laser Fired!\n");

        if (currentAmmo > 0) 
        {
            currentAmmo--;
        }
    }
}
