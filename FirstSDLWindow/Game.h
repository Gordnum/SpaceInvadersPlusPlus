#pragma once
#include <SDL.h>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "UFO.h"

class Game
{
	private:
    void handleEvents();
    void update();
    void render();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Player* player;
    Bullet* bullet;
    Enemy* enemy;
    UFO* ufo;
    std::vector<Enemy*> enemies;

    int enemyDirection = 1;
    int enemySpeed = 10;
    unsigned int lastMoveTime = 0;
    unsigned int moveInterval = 700;
    unsigned int lastUFOSpawnTime = 0;
    unsigned int ufoSpawnInterval = 13000;
    bool hasBounced = false;

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