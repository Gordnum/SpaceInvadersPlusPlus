#include "Game.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Game::Game() 
	 :window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), bullet(nullptr), enemy(nullptr), ufo(nullptr), 
	scoreManager(nullptr), isGameOver(false), gameOverStartTime(0){}


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

	if(TTF_Init() < 0) {
		std::cerr << "TTF could not initialize! " << TTF_GetError() << "\n";
		return false;
	}

	window = SDL_CreateWindow("SpaceInvaders++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	player = new Player(renderer);
	bullet = new Bullet(renderer);
	ufo = new UFO(renderer);
	scoreManager = new ScoreManager(renderer);

	scoreManager->loadHighScore("highscore.txt");

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			int x = 50 + col * (40 + spacingX); // change starting position
			int y = 150 + row * (20 + spacingY); // change starting height

			enemy = new Enemy(renderer);
			enemy->setRowIndex(row);  // this is key for tracking top row
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
	if (isGameOver) return;

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

	// Enemy shoot
	unsigned int currentEnemyShotTime = SDL_GetTicks();
	if (currentEnemyShotTime - lastEnemyShotTime >= enemyShootCooldown) {
		std::vector<Enemy*> shooters;
		for (Enemy* e : enemies) {
			if (e->isAlive() && e->isInTopRow()) {
				shooters.push_back(e);
			}
		}

		if (!shooters.empty()) {
			Enemy* shooter = shooters[rand() % shooters.size()];

			Bullet* b = new Bullet(renderer);
			SDL_Rect r = shooter->getRect();
			b->fireFrom(r.x + r.w / 2, r.y + r.h, true);
			enemyBullets.push_back(b);
		}

		lastEnemyShotTime = currentEnemyShotTime;
	}

	// other updates (player, bullet, collisions, etc.)
	for (auto& enemy : enemies)
	{
		if (bullet->isActive() && enemy->isAlive() &&
			checkCollision(bullet->getRect(), enemy->getRect()))
		{
			bullet->deactivate();
			enemy->destroy();
			scoreManager->addPoints(10);
		}

		if (enemy->isAlive())
			enemy->update();
	}

	// if hit with bullets
	for (auto& bullet : enemyBullets)
	{
		if (bullet->isActive() && checkCollision(bullet->getRect(), player->getRect()))
		{
			bullet->deactivate();
			player->loseLives();

			if (player->getLives() <= 0)
			{
				isGameOver = true;
				gameOverStartTime = SDL_GetTicks();
			}
		}
	}

	// if enemy gets too close
	for (auto& enemy : enemies) 
	{
		if (enemy->isAlive() && (enemy->getRect().y + enemy->getRect().h >= player->getRect().y)) 
		{
			isGameOver = true;
			gameOverStartTime = SDL_GetTicks();
			break; // Stop checking after one has triggered
		}
	}

	// update bullet enemy
	for (Bullet* b : enemyBullets) 
	{
		if (b->isActive()) b->update();
	}

	// bullet hit on ufo
	if(bullet->isActive() && ufo->isActive() &&
		checkCollision(bullet->getRect(), ufo->getRect()))
	{
		bullet->deactivate();
		ufo->deactivate();
		scoreManager->addPoints(100);
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

	// New Highscore
	if(isGameOver && !highScoreSaved)
	{
		scoreManager->saveHighScore("highscore.txt");
		highScoreSaved = true;
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
	scoreManager->render(renderer);

	for (auto& enemy : enemies)
	{
		if (enemy->isAlive()) enemy->render();
	}

	for (Bullet* b : enemyBullets) 
	{
		if (b->isActive()) b->render();
	}

	if (isGameOver)
	{
		unsigned int gameOverScreen = SDL_GetTicks();

		if(gameOverScreen - gameOverStartTime >= 2000) // time to activate the game over screen
		{
			SDL_Color color = { 255, 255, 255 }; // Red
			TTF_Font* font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 48);

			if (!font)
			{
				SDL_Log("Failed to load font: %s", TTF_GetError());
				return;
			}

			SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME OVER", color);
			if (!surface)
			{
				SDL_Log("Failed to create surface: %s", TTF_GetError());
				return;
			}

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_Rect dest =
			{
				SCREEN_WIDTH / 2 - surface->w / 2,
				SCREEN_HEIGHT / 2 - surface->h / 2,
				surface->w, surface->h
			};

			SDL_FreeSurface(surface);
			SDL_RenderCopy(renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
			TTF_CloseFont(font);

			SDL_RenderPresent(renderer);
			return; // Skip rendering rest of game
		}
		
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
	delete scoreManager;
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}