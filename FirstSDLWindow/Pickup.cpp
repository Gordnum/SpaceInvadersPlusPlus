#include "Pickup.h"

const int SCREEN_HEIGHT = 600;
constexpr int PLAYFIELD_UPPER_MARGIN = 75;
constexpr int PLAYFIELD_BOTTOM_MARGIN = 90;
constexpr int BORDER_THICKNESS = 4;
constexpr int PLAYFIELD_TOP_Y = PLAYFIELD_UPPER_MARGIN + BORDER_THICKNESS;
constexpr int PLAYFIELD_BOTTOM_Y = SCREEN_HEIGHT - PLAYFIELD_BOTTOM_MARGIN;

Pickup::Pickup(SDL_Renderer* renderer, int x, int y, WeaponType type)
	   :renderer(renderer), type(type), collected(false), dropSpeed(175.0f)
{
	rect = { x, y, 15, 15};
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
	else if(type == WeaponType::RAPID_SHOT)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
	
}

void Pickup::update(float deltaTime)
{ 
	if (collected) return;

	rect.y += static_cast<int>(dropSpeed * deltaTime);

	if (rect.y + rect.h >= PLAYFIELD_BOTTOM_Y)
		collected = true;
}

SDL_Rect Pickup::getRect() const { return rect; }
WeaponType Pickup::getType() const { return type; }
bool Pickup::isCollected() const { return collected; }
void Pickup::collect() { collected = true; }