#include "Enemy.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

std::map<EnemyType, std::vector<SDL_Texture*>> Enemy::textures;
std::vector<SDL_Texture*> Enemy::deathTextures;

Enemy::Enemy(SDL_Renderer* renderer, EnemyType enemyType)
	  :renderer(renderer), alive(true), rowIndex(0), enemyType(enemyType), animationFrame(0)
{
	rect = { 100, 50, 40, 22 };
}

std::vector<std::unique_ptr<Enemy>> Enemy::createFormation(SDL_Renderer* renderer, int rows, int cols, int spacingX, int spacingY)
{
	std::vector<std::unique_ptr<Enemy>> formation;

	for (int row = 0; row < rows; ++row)
	{
		EnemyType type;

		if (rows == 6)
		{
			if (row == 0 || row == 1)
				type = EnemyType::SQUID;
			else if (row == 2 || row == 3)
				type = EnemyType::CRAB;
			else
				type = EnemyType::OCTOPUS;
		}
		else 
		{
			if (row == 0)
				type = EnemyType::SQUID;
			else if (row == 1 || row == 2)
				type = EnemyType::CRAB;
			else
				type = EnemyType::OCTOPUS;
		}

		for (int col = 0; col < cols; ++col)
		{
			int x = 30 + col * (40 + spacingX);
			int y = 125 + row * (20 + spacingY);

			auto enemy = std::make_unique<Enemy>(renderer, type);
			enemy->setPosition(x, y);
			enemy->setRowIndex(row);
			formation.push_back(std::move(enemy));
		}
	}

	return formation;
}

void Enemy::LoadTextures(SDL_Renderer* renderer)
{
	std::string base = getExeDir();

	std::map<EnemyType, std::vector<std::string>> texturePaths = 
	{
		{ EnemyType::SQUID,   { (base + "Assets\\Textures\\squid_1.png").c_str(), (base + "Assets\\Textures\\squid_2.png").c_str() } },
		{ EnemyType::OCTOPUS, { (base + "Assets\\Textures\\octopus_1.png").c_str(), (base + "Assets\\Textures\\octopus_2.png").c_str() } },
		{ EnemyType::CRAB,    { (base + "Assets\\Textures\\crab_1.png").c_str(), (base + "Assets\\Textures\\crab_2.png").c_str() } }
	};

	for (auto& pair : texturePaths)
	{
		EnemyType type = pair.first;
		for (const std::string& path : pair.second)
		{
			SDL_Surface* surface = IMG_Load(path.c_str());
			if (!surface) 
			{
				SDL_Log("Failed to load enemy image: %s", IMG_GetError());
				continue;
			}
			SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			if (tex) 
			{
				textures[type].push_back(tex);
			}
		}
	}

	std::vector<std::string> deathPaths = 
	{
		(base + "Assets\\Textures\\enemy_death_1.png").c_str(),
		(base + "Assets\\Textures\\enemy_death_2.png").c_str()
	};

	for (const std::string& path : deathPaths) 
	{
		SDL_Surface* surface = IMG_Load(path.c_str());
		if (!surface) 
		{
			SDL_Log("Failed to load death image: %s", IMG_GetError());
			continue;
		}

		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		if (tex)
			deathTextures.push_back(tex);
	}
}

void Enemy::FreeTextures()
{
	for (auto& pair : textures)
	{
		for (SDL_Texture* tex : pair.second)
		{
			SDL_DestroyTexture(tex);
		}
		pair.second.clear();
	}
	textures.clear();

	for (SDL_Texture* tex : deathTextures) 
	{
		SDL_DestroyTexture(tex);
	}
	deathTextures.clear();
}

void Enemy::update(float deltaTime)
{
	if (dying && !finishedDeathAnimation) 
	{
		deathTimer += deltaTime;

		if (deathTimer >= deathFrameDuration) 
		{
			deathTimer = 0.0f;
			deathFrameIndex++;

			if (deathFrameIndex >= static_cast<int>(deathTextures.size())) 
			{
				finishedDeathAnimation = true;
			}
		}
	}

	if (origin == EnemyOrigin::BOSS_SPAWNED)
	{
		if (rect.x + rect.w < 0 || rect.x > SCREEN_WIDTH)
			destroy();
	}
}

void Enemy::render()
{
	if (dying) 
	{
		if (!finishedDeathAnimation && deathFrameIndex < deathTextures.size()) 
		{
			SDL_Texture* currentDeathFrame = deathTextures[deathFrameIndex];
			SDL_RenderCopy(renderer, currentDeathFrame, nullptr, &rect);
		}
		return; // Do not render normal enemy texture
	}

	if (!alive && !dying) return;

	if (!textures[enemyType].empty()) 
	{
		SDL_Texture* currentTexture = textures[enemyType][animationFrame];
		SDL_RenderCopy(renderer, currentTexture, nullptr, &rect);
	}
	else 
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
}

void Enemy::setRect(SDL_Rect r) { rect = r; }

SDL_Rect Enemy::getRect() const { return rect; }

void Enemy::destroy() 
{
	if (!dying) 
	{
		dying = true;
		deathTimer = 0.0f;
		deathFrameIndex = 0;
		alive = false;
	}
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

void Enemy::setOrigin(EnemyOrigin o) { origin = o; }
void Enemy::advanceAnimation()
{
	if (!textures[enemyType].empty())
		animationFrame = (animationFrame + 1) % textures[enemyType].size();
}

bool Enemy::isAlive() const { return alive; }
EnemyType Enemy::getType() const { return enemyType; }
EnemyOrigin Enemy::getOrigin() const { return origin; }
int Enemy::getX() const { return rect.x + rect.w / 2; };
int Enemy::getY() const { return rect.y; };