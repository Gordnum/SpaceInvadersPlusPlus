#pragma once
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "WeaponType.h"
#include "Pickup.h"

class WeaponInventory
{
private:
	std::unordered_map<WeaponType, int> ammoMap;
	std::vector<WeaponType> ownedWeapons;
	int currentIndex;
	TTF_Font* font;
	float animationOffset;
	float animationSpeed;
	bool isAnimating;
	int animationDirection;
	float deltaTime;

public:
	WeaponInventory();

	void addWeapon(WeaponType type, int ammo);
	void useAmmo(WeaponType type);
	int getAmmo(WeaponType type) const;
	bool hasWeapon(WeaponType type) const;

	void renderWeaponHUD(SDL_Renderer* renderer);
	void setCurrentWeapon(WeaponType type);
	void swapToNextWeapon();
	void swapToPreviousWeapon();
	void startWeaponSwapAnimation(int direction);
	void update();
	void updateAnimation(float deltaTime);
	void updateDeltaTime(float dt);
	WeaponType getCurrentWeapon() const;
	std::vector<WeaponType> randomizeWeapon();
};