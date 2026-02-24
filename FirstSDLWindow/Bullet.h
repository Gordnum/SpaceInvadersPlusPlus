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
	float enemyBulletSpeed;
	float playerBulletSpeed;
	WeaponType currentWeapon;
	int ammo;
	int rowIndex = -1;

public:
	Bullet(SDL_Renderer* renderer);
	void fire(int x, int y, WeaponType weapon);
	void fireFrom(int x, int y, bool fromEnemy);
	void setEnemyBulletSpeed(float s);
	void update(float deltaTime);
	void render();
	SDL_Rect getRect() const;

	WeaponType getCurrentWeapon() const;
	int getAmmo() const;
	void useAmmo();

	bool isActive() const;
	void deactivate();

	void setRowIndex(int row);
	int getRowIndex() const;

};