#include "Bullet.h"

// note: if rect.y + == going upward, else if rect.y - == going downwards
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Bullet::Bullet(SDL_Renderer* renderer)
	   :renderer(renderer), active(false), bulletIsFromEnemy(false)
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
	if(active)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
}