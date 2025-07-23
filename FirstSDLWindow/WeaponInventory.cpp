#include "WeaponInventory.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

WeaponInventory::WeaponInventory() 
				:currentIndex(0)
{
	ammoMap[WeaponType::DEFAULT] = 0;
	ownedWeapons.push_back(WeaponType::DEFAULT);
}

void WeaponInventory::addWeapon(WeaponType type, int ammo)
{
	if (!hasWeapon(type))
		ownedWeapons.push_back(type);

	ammoMap[type] += ammo;
}

void WeaponInventory::useAmmo(WeaponType type)
{
	if (ammoMap[type] > 0)
	{
		ammoMap[type]--;

		// auto-remove when ammo is depleted
		if (ammoMap[type] == 0 && type != WeaponType::DEFAULT)
		{
			// Remove from ownedWeapons
			for (auto it = ownedWeapons.begin(); it != ownedWeapons.end(); ++it)
			{
				if (*it == type)
				{
					unsigned int removedIndex = std::distance(ownedWeapons.begin(), it);
					ownedWeapons.erase(it);

					// Adjust currentIndex safely
					if (currentIndex >= ownedWeapons.size())
						currentIndex = 0;

					break;
				}
			}

			ammoMap.erase(type);
		}
	}
}

int WeaponInventory::getAmmo(WeaponType type) const
{
	auto it = ammoMap.find(type);
	return it != ammoMap.end() ? it->second : 0;
}

bool WeaponInventory::hasWeapon(WeaponType type) const
{
	return ammoMap.find(type) != ammoMap.end();
}

void WeaponInventory::setCurrentWeapon(WeaponType type)
{
	// Only switch if the weapon is in the inventory
	for (unsigned int i = 0; i < ownedWeapons.size(); ++i)
	{
		if (ownedWeapons[i] == type)
		{
			currentIndex = i;
			SDL_Log("Current Weapon: %d", ownedWeapons[currentIndex]);
			return;
		}
	}
}

void WeaponInventory::swapToNextWeapon()
{
	if (ownedWeapons.size() <= 1) return;
	currentIndex = (currentIndex + 1) % ownedWeapons.size();
	SDL_Log("Current Weapon: %d", ownedWeapons[currentIndex]);
}

void WeaponInventory::swapToPreviousWeapon()
{
	if (ownedWeapons.size() <= 1) return;
	currentIndex = (currentIndex - 1 + ownedWeapons.size()) % ownedWeapons.size();
	SDL_Log("Current Weapon: %d", ownedWeapons[currentIndex]);
}

WeaponType WeaponInventory::getCurrentWeapon() const { return ownedWeapons[currentIndex]; }

std::vector<WeaponType> WeaponInventory::randomizeWeapon()
{
	std::vector<WeaponType> randomType =
	{
		WeaponType::PIERCING_SHOT,
		WeaponType::BOMB_SHOT
	};

	return randomType;
};