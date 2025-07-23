#pragma once
#include "WeaponType.h"
#include "Pickup.h"
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <ctime>

class WeaponInventory
{
	private:
		std::unordered_map<WeaponType, int> ammoMap;
		std::vector<WeaponType> ownedWeapons;
		int currentIndex;

	public:
		WeaponInventory();

		void addWeapon(WeaponType type, int ammo);
		void useAmmo(WeaponType type);
		int getAmmo(WeaponType type) const;
		bool hasWeapon(WeaponType type) const;

		void setCurrentWeapon(WeaponType type);
		void swapToNextWeapon();
		void swapToPreviousWeapon();
		WeaponType getCurrentWeapon() const;
		std::vector<WeaponType> randomizeWeapon();
};