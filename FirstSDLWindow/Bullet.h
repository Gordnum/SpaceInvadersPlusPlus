#pragma once
#include <SDL.h>
#include "WeaponType.h"

class Bullet
{
private:
	SDL_Rect rect;
	SDL_Renderer* renderer;
	bool active;
	bool bulletIsFromEnemy;
	WeaponType currentWeapon;
	int ammo;

public:
	Bullet(SDL_Renderer* renderer);
	void fire(int x, int y, WeaponType weapon);
	void fireFrom(int x, int y, bool fromEnemy); // Declare
	void update();
	void render();
	SDL_Rect getRect() const;

	void setWeapon(WeaponType type, int ammoAmount);
	WeaponType getCurrentWeapon() const;
	int getAmmo() const;
	void useAmmo();

	bool isActive() const;
	void deactivate();
};