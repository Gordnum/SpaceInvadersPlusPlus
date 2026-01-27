#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <windows.h>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "UFO.h"
#include "Boss.h"
#include "ScoreManager.h"
#include "ComboManager.h"
#include "BulletManager.h"
#include "WaveManager.h"
#include "MenuManager.h"
#include "SoundManager.h"
#include "WeaponType.h"
#include "WeaponInventory.h"
#include "Pickup.h"

enum class GameMode 
{
    CAMPAIGN,
    ENDLESS
};

class Game
{
	private:
        void handleEvents();
        void update();
        void render();

        GameMode currentMode;
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool isRunning;
        bool isGameOver;

        std::unique_ptr<Player> player;
        std::unique_ptr<Bullet> bullet;
        std::unique_ptr<Enemy> enemy;
        std::unique_ptr<UFO> ufo;
        std::unique_ptr<Boss> boss;
        std::unique_ptr<ComboManager> comboManager;
        std::unique_ptr<WaveManager> waveManager;
        std::unique_ptr<WeaponInventory> weaponInventory;
        std::unique_ptr<ScoreManager> scoreManager;
        std::unique_ptr<BulletManager> bulletManager;
        std::unique_ptr<MenuManager> menuManager;

        std::vector<std::unique_ptr<Enemy>> enemies;
        std::vector<std::unique_ptr<Pickup>> pickups;
        std::vector<std::unique_ptr<Bullet>> enemyBullets;
        std::vector<std::unique_ptr<BossBullet>> bossBullets;

        int enemyDirection = 1;
        unsigned int lastMoveTime = 0;
        unsigned int moveInterval = 700;
        unsigned int lastUFOSpawnTime = 0;
        unsigned int ufoSpawnInterval = 13000;
        bool hasBounced = false;

        unsigned int lastEnemyShotTime = 0;
        const unsigned int enemyShootCooldown = 2000; // every 2 seconds
        unsigned int gameOverStartTime;
        bool highScoreSaved = false;

        unsigned int fpsTimer = SDL_GetTicks();
        int frameCount = 0;

        float deltaTime;
        unsigned int lastTicks;

        float calculateDeltaTime();

        struct TripmineExplosion
        {
            int rowY;
            unsigned int triggerTime;
            unsigned int renderUntil;
            bool exploded = false;
            bool hitBoss = false;
        };

        std::vector<TripmineExplosion> pendingTripmines;

        //bounce threshold
        const int enemyMinX = 20;
        const int enemyMaxX = 780;
        const int enemyDropY = 15;

        //piercing shot flash screen
        bool flashActive = false;
        unsigned int flashStartTime = 0;
        const unsigned int flashDuration = 1500;

	public:
        Game();
        ~Game();
        bool init();
        void run();
        void clean();
};