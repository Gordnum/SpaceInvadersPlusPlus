#include "Game.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Game::Game() 
	 :window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), bullet(nullptr), enemy(nullptr), ufo(nullptr), 
	scoreManager(nullptr), bulletManager(nullptr), isGameOver(false), gameOverStartTime(0){}


Game::~Game() {clean();}

// ADD
bool Game::init()
{
	int rows = 5;
	int cols = 11;
	int spacingX = 10;
	int spacingY = 10;
	srand(static_cast<unsigned int>(time(nullptr)));
	
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
	bulletManager = new BulletManager(renderer);
	pickups.push_back(new Pickup(renderer, 50 + (rand() % (SCREEN_WIDTH - 50)), -100, WeaponType::PIERCING_SHOT));

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

		if(event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_e)
				inventory.swapToNextWeapon();
			else if (event.key.keysym.sym == SDLK_q)
				inventory.swapToPreviousWeapon();
		}

		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE && bullet->isActive() == false)
		{
			WeaponType currentWeapon = inventory.getCurrentWeapon();
			int ammo = inventory.getAmmo(currentWeapon);

			if (currentWeapon == WeaponType::DEFAULT || ammo > 0)
			{
				bulletManager->fire(player->getX(), player->getY(), currentWeapon, ammo);
				if (currentWeapon != WeaponType::DEFAULT)
					inventory.useAmmo(currentWeapon);
			}
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
		bool willBounce = false;

		// Check if any enemy will go out of bounds on next move
		for (auto& enemy : enemies)
		{
			if (!enemy->isAlive()) continue;

			SDL_Rect r = enemy->getRect();
			int nextX = r.x + enemyDirection * enemySpeed;

			if (nextX <= 0 || (nextX + r.w) >= SCREEN_WIDTH)
			{
				willBounce = true;
				break;
			}
		}

		if (willBounce && !hasBounced)
		{
			enemyDirection *= -1;
			for (auto& enemy : enemies)
			{
				if (enemy->isAlive())
					enemy->move(0, 20); // move down instead of sideways
			}
			hasBounced = true;

			if (moveInterval > 100)
				moveInterval -= 60;
		}
		else
		{
			for (auto& enemy : enemies)
			{
				if (enemy->isAlive())
					enemy->move(enemyDirection * enemySpeed, 0);
			}
			hasBounced = false;
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

	// bullet hit enemy
	for(Bullet* b: bulletManager->getBullets())
	{
		for (auto& enemy : enemies)
		{
			if (!enemy->isAlive())
			{
				enemy->update();
				continue;
			}

			if (b->isActive() && enemy->isAlive() &&
				checkCollision(b->getRect(), enemy->getRect()))
			{
				if (b->getCurrentWeapon() == WeaponType::PIERCING_SHOT)
				{
					enemy->destroy();
				}
				else if(b->getCurrentWeapon() == WeaponType::BOMB_SHOT)
				{
					enemy->destroy();

					const int BOMB_RADIUS = 50;
					SDL_Rect center = enemy->getRect(); // center of explosion

					// Affect nearby enemies
					for (auto& e2 : enemies)
					{
						if (e2 != enemy && e2->isAlive())
						{
							SDL_Rect r2 = e2->getRect();

							int dx = abs((r2.x + r2.w / 2) - (center.x + center.w / 2));
							int dy = abs((r2.y + r2.h / 2) - (center.y + center.h / 2));

							if (dx <= BOMB_RADIUS && dy <= BOMB_RADIUS)
							{
								e2->destroy();
								int baseScore = 10;
								int earnedScore = static_cast<int>(baseScore * comboManager.getMultiplier());
								scoreManager->addPoints(earnedScore);
								comboManager.onEnemyKilled();
							}
						}
					}

					b->deactivate();
				}
				else
				{
					b->deactivate();
					enemy->destroy();
				}

				int baseScore = 10;
				int earnedScore = static_cast<int>(baseScore * comboManager.getMultiplier());
				scoreManager->addPoints(earnedScore);
				comboManager.onEnemyKilled();
				if (scoreManager->spawnPickup())
					pickups.push_back(new Pickup(renderer, 50 + (rand() % (SCREEN_WIDTH - 50)), -100, WeaponType::BOMB_SHOT));
			}
		}
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
	for (Bullet* bullet : enemyBullets) 
	{
		if (bullet->isActive()) bullet->update();
	}

	// bullet hit on ufo

	for (Bullet* b : bulletManager->getBullets())
	{
		if (b->isActive() && ufo->isActive() &&
			checkCollision(b->getRect(), ufo->getRect()))
		{
			if (b->getCurrentWeapon() == WeaponType::PIERCING_SHOT)
			{
				ufo->deactivate();
				
			}
			else if(b->getCurrentWeapon() == WeaponType::BOMB_SHOT)
			{
				ufo->deactivate();
			}
			else
			{
				b->deactivate();
				ufo->deactivate();
			}

			int baseScore = 100;
			int earnedScore = static_cast<int>(baseScore * comboManager.getMultiplier());
			scoreManager->addPoints(earnedScore);
			comboManager.onEnemyKilled();
			if (scoreManager->spawnPickup())
				pickups.push_back(new Pickup(renderer, ufo->getX(), ufo->getY(), WeaponType::PIERCING_SHOT));
		}
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

	// PICKUPS
	for (auto& pickup : pickups)
	{
		if (!pickup->isCollected() && checkCollision(player->getRect(), pickup->getRect()))
		{
			if (pickup->getType() == WeaponType::PIERCING_SHOT)
				inventory.addWeapon(WeaponType::PIERCING_SHOT, 2); // set ammo
			else if(pickup->getType() == WeaponType::BOMB_SHOT)
				inventory.addWeapon(WeaponType::BOMB_SHOT, 1);

			inventory.setCurrentWeapon(pickup->getType());
			pickup->collect();
		}
		pickup->update();
	}

	// NEW HIGHSCORE
	if(isGameOver && !highScoreSaved)
	{
		scoreManager->saveHighScore("highscore.txt");
		highScoreSaved = true;
	}

	ufo->update();
	player->update();
	bullet->update();
	bulletManager->update();
	comboManager.update();
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
	bulletManager->render();
	comboManager.render(renderer);

	for (auto& enemy : enemies)
	{
		if (enemy->isAlive()) enemy->render();
	}

	for (auto& pickup : pickups)
	{
		pickup->render();
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
			SDL_Color color = { 255, 255, 255 };
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
			return;
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
	delete ufo;
	delete scoreManager;
	delete bulletManager;
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}