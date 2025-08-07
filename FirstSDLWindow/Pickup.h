#pragma once
#include <SDL.h>
#include "WeaponType.h"

class Pickup
{

	private:
		SDL_Rect rect;
		SDL_Renderer* renderer;
		WeaponType type;
		bool collected;
		float dropSpeed;

	public:
		Pickup(SDL_Renderer* renderer, int x, int y, WeaponType type);
		void render();
		void update(float deltaTime);
		SDL_Rect getRect() const;
		WeaponType getType() const;
		bool isCollected() const;
		void collect();

};