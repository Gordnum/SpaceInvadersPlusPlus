#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <memory>
#include "BossBullet.h"
#include "SoundManager.h"
#include "Utils.h"

struct BossDeathExplosion
{
    SDL_Rect rect;
    int frame = 0;
    Uint32 lastFrameTime = 0;
    Uint32 startTime = 0;
};

class Boss
{
	private:
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        std::vector<SDL_Texture*> bossDeath;

        SDL_Rect rect;
        int health;
        const int maxHealth = 400;
        float speed;
        int direction;
        bool active;

        bool dying = false;
        bool visible = true;
        unsigned int deathStartTime = 0;
        unsigned int lastExplosionSpawn = 0;
        unsigned int blinkDuration = 5000;
        unsigned int blinkInterval = 100;

        std::vector<BossDeathExplosion> explosions;

        unsigned int lastShotTime = 0;
        const unsigned int shootCooldown = 5000;
        unsigned int lastSummonTime = 0;
        const unsigned int summonCooldown = 4000;

        int hoverChannel = -1;

	public:
        Boss(SDL_Renderer* renderer);
        ~Boss();

        void update(float deltaTime, SDL_Renderer* renderer, std::vector<std::unique_ptr<BossBullet>>& bullets);
        void render(SDL_Renderer* renderer);

        void startDeath();
        void updateDeath();
        bool isDying() const;
        bool isDeathFinished() const;

        void activate();
        void deactivate();
        bool isActive() const;
        bool isDefeated() const;
        bool shouldSpawnEnemies();

        void takeDamage(int amount);
        SDL_Rect getRect() const;
};