#pragma once
#include <iostream>
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
#include "SoundManager.h"

class WeaponInventory
{
	private:
		std::unordered_map<WeaponType, int> ammoMap;
		std::vector<WeaponType> ownedWeapons;
		int currentIndex;
		TTF_Font* font;
		TTF_Font* smallFont;
		TTF_Font* largeFont;
		float animationOffset;
		float animationSpeed;
		bool isAnimating;
		int animationDirection;
		float deltaTime;
		unsigned int lastWeaponSwitchSoundTime = 0;
		static constexpr unsigned int WEAPON_SWITCH_SOUND_COOLDOWN_MS = 120;

	public:
		WeaponInventory();
		~WeaponInventory();

		void addWeapon(WeaponType type, int ammo);
		void useAmmo(WeaponType type);
		int getAmmo(WeaponType type) const;
		bool hasWeapon(WeaponType type) const;

		void renderWeaponHUD(SDL_Renderer* renderer);
		void setCurrentWeapon(WeaponType type);
		void swapToNextWeapon();
		void swapToPreviousWeapon();
		void startWeaponSwapAnimation(int direction);
		void update(float deltaTime);
		WeaponType getCurrentWeapon() const;
		std::vector<WeaponType> randomizeWeapon();
};