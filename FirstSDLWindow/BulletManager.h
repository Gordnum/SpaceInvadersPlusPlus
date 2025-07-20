#pragma once
#include <vector>
#include "Bullet.h"

class BulletManager
{
	private:
		SDL_Renderer* renderer;
		std::vector<Bullet*> bullets;

	public:
		BulletManager(SDL_Renderer* renderer);
		~BulletManager();

		std::vector<Bullet*>& getBullets();
		void fire(int x, int y, WeaponType weaponType, int ammo);
		void update();
		void render();
		void clear();
};