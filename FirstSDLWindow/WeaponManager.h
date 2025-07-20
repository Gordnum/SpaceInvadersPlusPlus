#pragma once
#include "WeaponType.h"

class WeaponManager 
{
public:
    WeaponManager();

    void setWeapon(WeaponType type, int ammo = 0); // -1 = unlimited
    void update();
    void render(SDL_Renderer* renderer); // optional, for visual UI
    void onShoot(); // called when player presses shoot

    WeaponType getCurrentWeapon() const;
    bool canShoot() const;

private:
    WeaponType currentWeapon;
    int currentAmmo;
};
