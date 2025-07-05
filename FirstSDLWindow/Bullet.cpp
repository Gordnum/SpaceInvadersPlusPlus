#include "Bullet.h"

Bullet::Bullet(SDL_Renderer* renderer)
	   :renderer(renderer), active(false)
{
	rect = { 0, 0, 5, 10 };
}

SDL_Rect Bullet::getRect() const { return rect; }
bool Bullet::isActive() const { return active; }
void Bullet::deactivate() { active = false; }

void Bullet::fire(int x, int y) // player
{
	if(!active)
	{
		rect.x = x - rect.w / 2;
		rect.y = y;
		active = true;
	}
}

void Bullet::fireFrom(int x, int y) // enemy
{
	rect.x = x - rect.w / 2;
	rect.y = y;
	active = true;
}

void Bullet::update()
{
	if(active)
	{
		rect.y -= 5;
		if (rect.y < 0) active = false;
	}
}

void Bullet::render()
{
	if(active)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
}