#pragma once
#include <vector>
#include <memory>
#include "Bullet.h"

class BulletManager
{
	private:
		SDL_Renderer* renderer;
		std::vector<std::unique_ptr<Bullet>> bullets;

	public:
		BulletManager(SDL_Renderer* renderer);
		~BulletManager();

		std::vector<std::unique_ptr<Bullet>>& getBullets();
		bool fire(int x, int y, WeaponType weaponType, int ammo);
		void update(float deltaTime);
		void render();
		void clear();
};