#include "Pickup.h"

Pickup::Pickup(SDL_Renderer* renderer, int x, int y, WeaponType type)
	   :renderer(renderer), type(type), collected(false)
{
	rect = { 200, 500, 20, 20 };
}

void Pickup::render()
{
	if (collected) return;

	if(type == WeaponType::PIERCING_SHOT)
	{
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
	
}

void Pickup::update() { /* to be continued */ }

SDL_Rect Pickup::getRect() const { return rect; }
WeaponType Pickup::getType() const { return type; }
bool Pickup::isCollected() const { return collected; }
void Pickup::collect() { collected = true; }