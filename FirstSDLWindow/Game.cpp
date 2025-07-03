#include "Game.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Game::Game() 
	 :window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), bullet(nullptr), enemy(nullptr), ufo(nullptr){}


Game::~Game() {clean();}

// ADD
bool Game::init()
{
	int rows = 5;
	int cols = 11;
	int spacingX = 10;
	int spacingY = 10;
	
	if(SDL_INIT_EVERYTHING < 0)
	{
		std::cerr << "SDL init failed";
		return false;
	}

	window = SDL_CreateWindow("SpaceInvaders++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	player = new Player(renderer);
	bullet = new Bullet(renderer);
	ufo = new UFO(renderer);
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			int x = 50 + col * (40 + spacingX); // change starting position
			int y = 150 + row * (20 + spacingY); // change starting height

			enemy = new Enemy(renderer);
			enemy->setPosition(x, y);
			enemies.push_back(enemy);
		}
	}

	isRunning = true;
	return true;
}

static bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) 
{
	return SDL_HasIntersection(&a, &b);
}

void Game::handleEvents() 
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			isRunning = false;
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
		{
			bullet->fire(player->getX(), player->getY());
		}

		player->handleEvent(event);
	}
}

// ADD
void Game::update()
{
	unsigned int currentTime = SDL_GetTicks();

	if (currentTime - lastMoveTime >= moveInterval)
	{
		int leftEdge = SCREEN_WIDTH;
		int rightEdge = 0;

		for (auto& enemy: enemies)
		{
			if (!enemy->isAlive()) continue;
			SDL_Rect r = enemy->getRect();
			if (r.x < leftEdge) leftEdge = r.x;
			if (r.x + r.w > rightEdge) rightEdge = r.x + r.w;
		}

		// BOUNCE: Only if at edge and we haven't already bounced
		if ((leftEdge <= 0 || rightEdge >= SCREEN_WIDTH) && !hasBounced)
		{
			enemyDirection *= -1;
			for (auto& enemy: enemies)
			{
				if (enemy->isAlive())
					enemy->move(0, 30);  // go down once
			}
			hasBounced = true; // prevent repeat bounce

			if (moveInterval > 100) // enemy speed after bounce
			{
				moveInterval -= 60;
			}
				
		}
		else
		{
			// MOVE: horizontal movement
			for (auto& enemy: enemies)
			{
				if (enemy->isAlive())
					enemy->move(enemyDirection * enemySpeed, 0);
			}

			hasBounced = false; // allow bouncing again next time
		}

		lastMoveTime = currentTime;
	}

	// other updates (player, bullet, collisions, etc.)
	for (auto& enemy : enemies)
	{
		if (bullet->isActive() && enemy->isAlive() &&
			checkCollision(bullet->getRect(), enemy->getRect()))
		{
			bullet->deactivate();
			enemy->destroy();
		}

		if (enemy->isAlive())
			enemy->update();
	}

	if(bullet->isActive() && ufo->isActive() &&
		checkCollision(bullet->getRect(), ufo->getRect()))
	{
		bullet->deactivate();
		ufo->deactivate();
		lastUFOSpawnTime = currentTime; // reset if ufo died
	}
		
	// UFO

	if(currentTime - lastUFOSpawnTime >= ufoSpawnInterval)
	{
			if(!ufo->isActive())
			{
				ufo->activate();
				lastUFOSpawnTime = currentTime;
			}
	}

	ufo->update();
	player->update();
	bullet->update();
}

// ADD
void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	player->render();
	bullet->render();
	ufo->render();

	for (auto& enemy : enemies)
	{
		if (enemy->isAlive()) enemy->render();
	}

	SDL_RenderPresent(renderer);
}

void Game::run()
{
	while (isRunning)
	{
		handleEvents();
		render();
		update();
		SDL_Delay(16);
	}
}

// DELETE
void Game::clean() 
{
	delete player;
	delete bullet;
	delete enemy;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}