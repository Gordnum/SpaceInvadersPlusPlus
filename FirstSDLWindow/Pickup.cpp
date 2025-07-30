#include "Pickup.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Pickup::Pickup(SDL_Renderer* renderer, int x, int y, WeaponType type)
	   :renderer(renderer), type(type), collected(false)
{
	rect = { x, y, 20, 20 };
}

void Pickup::render()
{
	if (collected) return;

	if(type == WeaponType::PIERCING_SHOT)
	{
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
	else if(type == WeaponType::BOMB_SHOT)
	{
		SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
	else if(type == WeaponType::TRIPMINE)
	{
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
	
}

void Pickup::update() 
{ 
	rect.y += 3;

	if (rect.y > SCREEN_HEIGHT)
		return;
}

SDL_Rect Pickup::getRect() const { return rect; }
WeaponType Pickup::getType() const { return type; }
bool Pickup::isCollected() const { return collected; }
void Pickup::collect() { collected = true; }