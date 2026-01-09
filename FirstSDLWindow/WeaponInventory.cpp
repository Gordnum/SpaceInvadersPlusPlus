#include "WeaponInventory.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

WeaponInventory::WeaponInventory() 
				:currentIndex(0), font(nullptr), animationOffset(0.0f), 
				 animationSpeed(5.0f), isAnimating(false), animationDirection(0), deltaTime(0.0f)
{
	smallFont = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 16);
	largeFont = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 20);
	font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 20);

	if (!font)
		SDL_Log("Failed to load ammo font: %s", TTF_GetError());
	if (!smallFont || !largeFont)
		SDL_Log("Failed to load weapon fonts: %s", TTF_GetError());
	
	ammoMap[WeaponType::DEFAULT] = 0;
	ownedWeapons.push_back(WeaponType::DEFAULT);
}

WeaponInventory::~WeaponInventory()
{
	if (smallFont) TTF_CloseFont(smallFont);
	if (largeFont) TTF_CloseFont(largeFont);
	if (font) TTF_CloseFont(font);
}

void WeaponInventory::addWeapon(WeaponType type, int ammo)
{
	if (!hasWeapon(type))
		ownedWeapons.push_back(type);

	ammoMap[type] += ammo;
}

void WeaponInventory::useAmmo(WeaponType type)
{
	//aound for switching weapons
	static auto canPlayWeaponSwitchSound = [&](unsigned int& lastTime)
	{
		unsigned int now = SDL_GetTicks();
		if (now - lastTime < WeaponInventory::WEAPON_SWITCH_SOUND_COOLDOWN_MS)
			return false;

		lastTime = now;
		return true;
	};

	if (ammoMap[type] > 0)
	{
		ammoMap[type]--;

		// auto-remove when ammo is depleted
		if (ammoMap[type] == 0 && type != WeaponType::DEFAULT)
		{
			bool wasCurrentWeapon = (getCurrentWeapon() == type);

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

			if (wasCurrentWeapon && !ownedWeapons.empty())
			{
				if (canPlayWeaponSwitchSound(lastWeaponSwitchSoundTime))
					SoundManager::playSound(weaponTypeToSwitchSound(getCurrentWeapon()));
			}
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

void WeaponInventory::startWeaponSwapAnimation(int direction)
{
	if (isAnimating || ownedWeapons.size() <= 1) return;

	isAnimating = true;
	animationDirection = direction;
	animationOffset = 0.0f;
}

void WeaponInventory::renderWeaponHUD(SDL_Renderer* renderer)
{
	if (ownedWeapons.size() <= 0) return; // no wheel for 1 weapon

	const int centerX = SCREEN_WIDTH - 610;
	const int baseY = SCREEN_HEIGHT - 36;
	const int padding = -2;

	unsigned int numWeapons = ownedWeapons.size();
	int halfRange = std::min(1, (int)numWeapons / 2);

	struct WeaponRenderInfo 
	{
		std::string name;
		int fontSize;
		int width;
		int height;
		SDL_Texture* texture;
	};

	std::vector<WeaponRenderInfo> renderList;
	int totalHeight = 0;

	// Prepare render info for -1, 0, +1
	for (int i = -halfRange; i <= halfRange; ++i)
	{
		int actualIndex = (currentIndex + i + numWeapons) % numWeapons;
		WeaponType type = ownedWeapons[actualIndex];

		std::string currentWeapon;
		switch (type)
		{
			case WeaponType::DEFAULT: currentWeapon = "DEFAULT"; break;
			case WeaponType::PIERCING_SHOT: currentWeapon = "PIERCING"; break;
			case WeaponType::BOMB_SHOT: currentWeapon = "BOMB"; break;
			case WeaponType::TRIPMINE: currentWeapon = "TRIPMINE"; break;
			case WeaponType::RAPID_SHOT: currentWeapon = "RPDSHOT"; break;
			default: currentWeapon = "UNKNOWN"; break;
		}

		bool isCenter = (i == 0 && !isAnimating);
		int fontSize = isCenter ? 20 : 16;

		TTF_Font* displayFont = isCenter ? largeFont : smallFont;
		if (!displayFont) continue;

		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Surface* surface = TTF_RenderText_Solid(displayFont, currentWeapon.c_str(), color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		renderList.push_back({ currentWeapon, fontSize, w, h, texture });
		totalHeight += h + padding;

		SDL_FreeSurface(surface);
	}

	// Remove last padding
	totalHeight -= padding;

	// Draw centered vertically
	if (renderList.empty()) return;
	float currentY = baseY - totalHeight / 2.0f - animationOffset * (renderList[0].height + padding); // shift up/down based on anim

	for (const auto& info : renderList)
	{
		SDL_Rect dst = 
		{
			centerX - info.width / 2,
			static_cast<int>(currentY),
			info.width,
			info.height
		};

		SDL_RenderCopy(renderer, info.texture, nullptr, &dst);
		currentY += info.height + padding;

		SDL_DestroyTexture(info.texture);
	}

	int ammo = getAmmo(getCurrentWeapon());

	std::string text = "Weapon: ";
	SDL_Color textColor = { 255, 255, 255, 255 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
	if (!surface)
	{
		SDL_Log("TTF Render Error: %s", TTF_GetError());
		return;
	}

	std::string ammoText = "Ammo: " + std::to_string(ammo);
	SDL_Color ammoColor = { 255, 255, 255, 255 };
	SDL_Surface* ammoSurface = TTF_RenderText_Solid(font, ammoText.c_str(), ammoColor);

	if(!ammoSurface)
	{
		SDL_Log("TTF Render Error: %s", TTF_GetError());
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Texture* ammoTexture = SDL_CreateTextureFromSurface(renderer, ammoSurface);
	SDL_FreeSurface(ammoSurface);

	SDL_Rect destRect = { 25, 550, 0, 0 };
	SDL_QueryTexture(texture, nullptr, nullptr, &destRect.w, &destRect.h);
	SDL_RenderCopy(renderer, texture, nullptr, &destRect);

	SDL_Rect ammoRect = { destRect.x + 250, 550, 0, 0 };
	SDL_QueryTexture(ammoTexture, nullptr, nullptr, &ammoRect.w, &ammoRect.h);
	SDL_RenderCopy(renderer, ammoTexture, nullptr, &ammoRect);

	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(ammoTexture);
}

void WeaponInventory::update(float deltaTime) 
{
	static auto canPlayWeaponSwitchSound = [&](unsigned int& lastTime) 
	{
		unsigned int now = SDL_GetTicks();
		if (now - lastTime < WeaponInventory::WEAPON_SWITCH_SOUND_COOLDOWN_MS)
			return false;

		lastTime = now;
		return true;
	};

	if (!isAnimating) return;

	float direction = animationDirection;
	animationOffset += direction * animationSpeed * deltaTime;

	if (std::abs(animationOffset) >= 1.0f - 0.001f)
	{
		isAnimating = false;
		animationOffset = 0.0f;
		if (direction > 0)
			swapToNextWeapon();
		else
			swapToPreviousWeapon();

		if (canPlayWeaponSwitchSound(lastWeaponSwitchSoundTime))
			SoundManager::playSound(weaponTypeToSwitchSound(getCurrentWeapon()));
	}
}

WeaponType WeaponInventory::getCurrentWeapon() const 
{ 
	try 
	{
		return ownedWeapons.at(currentIndex);
	}
	catch (const std::out_of_range& e) {
		std::cerr << "[getCurrentWeapon] Out of range: " << e.what() << std::endl;
		return WeaponType::DEFAULT; // fallback
	}
}

std::vector<WeaponType> WeaponInventory::randomizeWeapon()
{
	std::vector<WeaponType> randomType =
	{
		WeaponType::PIERCING_SHOT,
		WeaponType::BOMB_SHOT,
		WeaponType::TRIPMINE,
		WeaponType::RAPID_SHOT
	};

	return randomType;
};