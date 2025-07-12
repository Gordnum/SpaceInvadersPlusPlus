#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "UFO.h"
#include "ScoreManager.h"

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
    ScoreManager* scoreManager;
    std::vector<Enemy*> enemies;

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