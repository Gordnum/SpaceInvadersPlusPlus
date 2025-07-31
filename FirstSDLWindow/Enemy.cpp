#include "Enemy.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Enemy::Enemy(SDL_Renderer* renderer, EnemyType enemyType)
	  :renderer(renderer), alive(true), rowIndex(0), enemyType(enemyType)
{
	rect = { 100, 50, 40, 20 };
}

std::vector<Enemy*> Enemy::createFormation(SDL_Renderer* renderer, int rows, int cols, int spacingX, int spacingY)
{
	std::vector<Enemy*> formation;

	for (int row = 0; row < rows; ++row)
	{
		EnemyType type;
		if (row == 0)
			type = EnemyType::SQUID;
		else if (row == 1 || row == 2)
			type = EnemyType::OCTOPUS;
		else
			type = EnemyType::CRAB;

		for (int col = 0; col < cols; ++col)
		{
			int x = 30 + col * (40 + spacingX);
			int y = 150 + row * (20 + spacingY);

			Enemy* enemy = new Enemy(renderer, type);
			enemy->setPosition(x, y);
			enemy->setRowIndex(row);
			formation.push_back(enemy);
		}
	}

	return formation;
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
	
	switch(enemyType)
	{
        case EnemyType::SQUID:
			SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
			break;
		case EnemyType::OCTOPUS:
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			break;
		case EnemyType::CRAB:
			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
			break;
	}

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

EnemyType Enemy::getType() const { return enemyType; }