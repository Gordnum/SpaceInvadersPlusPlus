#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "UFO.h"
#include "ScoreManager.h"
#include "ComboManager.h"
#include "BulletManager.h"
#include "WeaponType.h"
#include "Pickup.h"

class Game
{
	private:
    void handleEvents();
    void update();
    void render();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    bool isGameOver;

    Player* player;
    Bullet* bullet;
    Enemy* enemy;
    UFO* ufo;
    ComboManager comboManager;
    ScoreManager* scoreManager;
    BulletManager* bulletManager;
    std::vector<Enemy*> enemies;
    std::vector<Pickup*> pickups;

    int enemyDirection = 1;
    int enemySpeed = 10;
    unsigned int lastMoveTime = 0;
    unsigned int moveInterval = 700;
    unsigned int lastUFOSpawnTime = 0;
    unsigned int ufoSpawnInterval = 13000;
    bool hasBounced = false;

    std::vector<Bullet*> enemyBullets;
    unsigned int lastEnemyShotTime = 0;
    const unsigned int enemyShootCooldown = 2000; // every 2 seconds
    unsigned int gameOverStartTime;
    bool highScoreSaved = false;

	public:
    Game();
    ~Game();
    bool init();
    void run();
    void clean();
};

/*
enum class GameState
{
    PLAY,
    EXIT
};
GameState state;
*/