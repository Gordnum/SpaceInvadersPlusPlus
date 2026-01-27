#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <memory>
#include "BossBullet.h"
#include "SoundManager.h"

class Boss
{
	private:
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        SDL_Rect rect;
        int health;
        const int maxHealth = 100;
        float speed;
        int direction;
        bool active;
        unsigned int lastShotTime = 0;
        const unsigned int shootCooldown = 5000;
        unsigned int lastSummonTime = 0;
        const unsigned int summonCooldown = 4000;

	public:
        Boss(SDL_Renderer* renderer);
        ~Boss();

        void update(float deltaTime, SDL_Renderer* renderer, std::vector<std::unique_ptr<BossBullet>>& bullets);
        void render(SDL_Renderer* renderer);

        void activate();
        void deactivate();
        bool isActive() const;
        bool isDefeated() const;
        bool shouldSpawnEnemies();

        void takeDamage(int amount);
        SDL_Rect getRect() const;
};