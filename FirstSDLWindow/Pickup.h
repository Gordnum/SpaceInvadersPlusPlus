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

	public:
		Pickup(SDL_Renderer* renderer, int x, int y, WeaponType type);
		void render();
		void update(); // For animations or future behavior
		SDL_Rect getRect() const;
		WeaponType getType() const;
		bool isCollected() const;
		void collect();

};