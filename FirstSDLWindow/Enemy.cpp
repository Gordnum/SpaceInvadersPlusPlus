#include "Enemy.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Enemy::Enemy(SDL_Renderer* renderer)
	  :renderer(renderer), alive(true), rowIndex(0)
{
	rect = { 100, 50, 40, 20 };
}

// STILL NEED TO BE FIXED BECAUSE IT'S UNUSED
void Enemy::update()
{
	/*unsigned int currentTime = SDL_GetTicks();
	if (currentTime - lastMoveTime >= moveInterval)
	{
		rect.x += speed;
		if (rect.x <= 0 || rect.x + rect.w >= 800)
			speed = -speed;

		lastMoveTime = currentTime;
	}*/
}

void Enemy::render()
{
	if (!alive) return;
	
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &rect);
}

SDL_Rect Enemy::getRect() const { return rect; }

void Enemy::destroy() 
{
	alive = false;
}

void Enemy::setPosition(int x, int y)
{
	rect.x = x;
	rect.y = y;
}

void Enemy::move(int dx, int dy) 
{
	rect.x += dx;
	rect.y += dy;
}

bool Enemy::isAlive() const
{
	return alive;
}