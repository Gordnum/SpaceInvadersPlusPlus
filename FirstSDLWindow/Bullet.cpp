#include "Bullet.h"

// note: if rect.y + == going upward, else if rect.y - == going downwards
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Bullet::Bullet(SDL_Renderer* renderer)
	:renderer(renderer), active(false), bulletIsFromEnemy(false), currentWeapon(WeaponType::DEFAULT), ammo(0)
{
	rect = { 0, 0, 5, 10 };
}

void Bullet::fire(int x, int y, WeaponType type) // player
{
	if(!active)
	{
		
		currentWeapon = type;

		if (type == WeaponType::PIERCING_SHOT && ammo > 0)
		{
			rect.x = x - rect.w / 2;
			rect.y = 0;
			rect.w = 10;
			rect.h = 500; // full screen height shot
			
		}
		else if(type == WeaponType::DEFAULT)
		{
			rect.x = x - rect.w / 2;
			rect.y = y;
			rect.w = 5;
			rect.h = 10;
		}
		
		active = true;
	}
}

void Bullet::fireFrom(int x, int y, bool fromEnemy) // enemy
{
	rect.x = x - rect.w / 2;
	rect.y = y;
	bulletIsFromEnemy = fromEnemy;
	active = true;
}

void Bullet::update()
{
	if(active && !bulletIsFromEnemy)
	{
		rect.y -= 5;
		if (rect.y < 0) active = false;
	}

	if(bulletIsFromEnemy)
		rect.y += 5;

	if (rect.y < 0 || rect.y > SCREEN_HEIGHT)
		deactivate();
}

void Bullet::render()
{
	if (!active) return;

	if(active)
	{
		if (currentWeapon == WeaponType::PIERCING_SHOT)
			SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // cyan 
		else if(currentWeapon == WeaponType::DEFAULT)
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow

		SDL_RenderFillRect(renderer, &rect);
	}
}

void Bullet::setWeapon(WeaponType type, int ammoAmount)
{
	currentWeapon = type;
	ammo = ammoAmount;
}

void Bullet::useAmmo() { if (ammo > 0) ammo--; }
WeaponType Bullet::getCurrentWeapon() const { return currentWeapon; }
int Bullet::getAmmo() const { return ammo; }

SDL_Rect Bullet::getRect() const { return rect; }
bool Bullet::isActive() const { return active; }
void Bullet::deactivate() { active = false; }