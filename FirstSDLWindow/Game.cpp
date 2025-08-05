#include "Game.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

Game::Game() 
	 :window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), bullet(nullptr), enemy(nullptr), ufo(nullptr), 
	  scoreManager(nullptr), weaponInventory(nullptr), comboManager(nullptr), waveManager(nullptr), menuManager(nullptr),
	  bulletManager(nullptr), isGameOver(false), gameOverStartTime(0){}


Game::~Game() {clean();}

float Game::calculateDeltaTime() 
{
	unsigned int currentTicks = SDL_GetTicks();
	float dt = (currentTicks - lastTicks) / 1000.0f;
	if (dt > 0.05f) dt = 0.05f;
	lastTicks = currentTicks;
	return dt;
}

// ADD
bool Game::init()
{
	int rows = 5;
	int cols = 11;
	int spacingX = 10;
	int spacingY = 10;
	srand(static_cast<unsigned int>(time(nullptr)));
	
	if (SDL_INIT_EVERYTHING < 0)
	{
		std::cerr << "SDL init failed";
		return false;
	}

	if (TTF_Init() < 0) 
	{
		std::cerr << "TTF could not initialize! " << TTF_GetError() << "\n";
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		std::cerr << "IMG_Init failed: " << IMG_GetError() << "\n";
		return false;
	}

	window = SDL_CreateWindow("SpaceInvaders++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	menuManager = new MenuManager(renderer);
	waveManager = new WaveManager();
	UFO::LoadTextures(renderer);
	Enemy::LoadTextures(renderer);
	player = new Player(renderer);
	player->loadWeaponTextures(renderer);
	bullet = new Bullet(renderer);
	ufo = new UFO(renderer);
	scoreManager = new ScoreManager(renderer);
	comboManager = new ComboManager();
	weaponInventory = new WeaponInventory();
	bulletManager = new BulletManager(renderer);
	scoreManager->loadHighScore("highscore.txt");

	isRunning = true;
	return true;
}

static bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) 
{
	return SDL_HasIntersection(&a, &b);
}

void Game::handleEvents() 
{
	if (menuManager->isInMainMenu())
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			bool startCampaign = false;
			bool startEndless = false;
			bool quitGame = false;

			menuManager->handleEvent(event, startCampaign, startEndless, quitGame);

			if (quitGame || event.type == SDL_QUIT)
				isRunning = false;
			else if (startCampaign)
			{
				currentMode = GameMode::CAMPAIGN;
				menuManager->setInMainMenu(false);
				waveManager->startWaveIntro();
				pickups.push_back(new Pickup(renderer, 400, -100, WeaponType::PIERCING_SHOT));
			}
			else if (startEndless)
			{
				currentMode = GameMode::ENDLESS;
				menuManager->setInMainMenu(false);
				waveManager->startWaveIntro();
			}
			return;
		}
	}

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			isRunning = false;

		if (!waveManager->getWaveIntro())
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_e)
					weaponInventory->startWeaponSwapAnimation(1);

				else if (event.key.keysym.sym == SDLK_q)
					weaponInventory->startWeaponSwapAnimation(-1);
			}

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE && bullet->isActive() == false)
			{
				WeaponType currentWeapon = weaponInventory->getCurrentWeapon();
				int ammo = weaponInventory->getAmmo(currentWeapon);

				if (currentWeapon == WeaponType::DEFAULT || ammo > 0)
				{
					bulletManager->fire(player->getX(), player->getY(), currentWeapon, ammo);
					if (currentWeapon != WeaponType::DEFAULT)
						weaponInventory->useAmmo(currentWeapon);
				}
			}
		}

		player->handleEvent(event);
	}
}

// ADD
void Game::update()
{
	if (isGameOver) return;

	deltaTime = calculateDeltaTime();

	for (auto& enemy : enemies) 
	{
		enemy->update(deltaTime);
	}

	ufo->update(deltaTime);

	// FPS Counter
	frameCount++;
	unsigned currentTicks = SDL_GetTicks();
	if (currentTicks - fpsTimer >= 1000) // 1000 ms = 1 second
	{
		std::cout << "FPS: " << frameCount << std::endl;
		frameCount = 0;
		fpsTimer = currentTicks;
	}

	// Enemy movement
	unsigned int currentTime = SDL_GetTicks();
	if (currentTime - lastMoveTime >= moveInterval)
	{
		bool willBounce = false;

		// Check if any enemy will go out of bounds on next move
		for (auto& enemy : enemies)
		{
			if (!enemy->isAlive()) continue;

			SDL_Rect r = enemy->getRect();
			int nextX = r.x + enemyDirection * static_cast<int>(waveManager->getEnemySpeed());

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
				{
					enemy->move(0, 15); // move down instead of sideways
					enemy->advanceAnimation();
				}
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
				{
					enemy->move(static_cast<int>(enemyDirection * waveManager->getEnemySpeed()), 0);
					enemy->advanceAnimation();
				}
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
			b->setEnemyBulletSpeed(waveManager->getProjectileSpeed());
			enemyBullets.push_back(b);
		}

		lastEnemyShotTime = currentEnemyShotTime;
	}

	// bullet hit enemy
	for(Bullet* b: bulletManager->getBullets())
	{
		int baseScore = 10;

		for (auto& enemy : enemies)
		{
			if (!enemy->isAlive()) continue;

			switch (enemy->getType())
			{
				case EnemyType::SQUID: baseScore = 50; break;
				case EnemyType::CRAB: baseScore = 30; break;
				case EnemyType::OCTOPUS: baseScore = 10; break;
			}

			if (b->isActive() && enemy->isAlive() &&
				checkCollision(b->getRect(), enemy->getRect()))
			{
				if (b->getCurrentWeapon() == WeaponType::PIERCING_SHOT)
				{
					enemy->destroy();
					int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
					scoreManager->addPoints(earnedScore);
					comboManager->onEnemyKilled();
				}
				else if(b->getCurrentWeapon() == WeaponType::BOMB_SHOT)
				{
					enemy->destroy();

					const int BOMB_RADIUS = 60;
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
								int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
								scoreManager->addPoints(earnedScore);
								comboManager->onEnemyKilled();
							}
						}
					}

					b->deactivate();
				}
				else if(b->getCurrentWeapon() == WeaponType::TRIPMINE)
				{
					int hitY = enemy->getRect().y;
					const int ROW_THRESHOLD = 10; // adjust based on enemy sprite height

					for (auto& otherEnemy : enemies)
					{
						if (!otherEnemy->isAlive()) continue;

						int enemyY = otherEnemy->getRect().y;

						if (std::abs(enemyY - hitY) < ROW_THRESHOLD)
						{
							otherEnemy->destroy(); // or setAlive(false), etc.
							int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
							scoreManager->addPoints(earnedScore);
							comboManager->onEnemyKilled();
						}
					}

					b->deactivate(); // remove tripmine bullet after explosion
				}
				else if(b->getCurrentWeapon() == WeaponType::DEFAULT || b->getCurrentWeapon() == WeaponType::RAPID_SHOT)
				{
					b->deactivate();
					enemy->destroy();
					int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
					scoreManager->addPoints(earnedScore);
					comboManager->onEnemyKilled();
				}

				if (scoreManager->spawnPickup())
				{
					auto randomWeapons = weaponInventory->randomizeWeapon();
					if (randomWeapons.empty())
						return; // or handle appropriately (e.g., no weapon to give)

					int index = rand() % randomWeapons.size();
					WeaponType randomWeapon = randomWeapons[index];

					int randomX = 50 + (rand() % (SCREEN_WIDTH - 100)); // safe margin
					pickups.push_back(new Pickup(renderer, randomX, -100, randomWeapon));
				}

				if (scoreManager->giveLive())
					player->plusLives();
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
				ufo->startDeathAnimation();
				ufo->deactivate();
			}
			else if(b->getCurrentWeapon() == WeaponType::BOMB_SHOT)
			{
				b->deactivate();
				ufo->startDeathAnimation();
				ufo->deactivate();
			}
			else if(b->getCurrentWeapon() == WeaponType::TRIPMINE)
			{
				b->deactivate();
				ufo->startDeathAnimation();
				ufo->deactivate();
			}
			else if(b->getCurrentWeapon() == WeaponType::DEFAULT || b->getCurrentWeapon() == WeaponType::RAPID_SHOT)
			{
				b->deactivate();
				ufo->startDeathAnimation();
				ufo->deactivate();
			}

			int baseScore = 200;
			int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
			scoreManager->addPoints(earnedScore);
			comboManager->onEnemyKilled();

			int index = rand() % weaponInventory->randomizeWeapon().size();
			WeaponType randomWeapon = weaponInventory->randomizeWeapon()[index];

			int randomX = 50 + (rand() % (SCREEN_WIDTH - 100)); // safe margin
			pickups.push_back(new Pickup(renderer, ufo->getX(), ufo->getY(), randomWeapon));
			if (scoreManager->spawnPickup())
				pickups.push_back(new Pickup(renderer, randomX, -100, randomWeapon));
			if (scoreManager->giveLive())
				player->plusLives();
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
				weaponInventory->addWeapon(WeaponType::PIERCING_SHOT, 1); // set ammo
			else if (pickup->getType() == WeaponType::BOMB_SHOT)
				weaponInventory->addWeapon(WeaponType::BOMB_SHOT, 1);
			else if (pickup->getType() == WeaponType::TRIPMINE)
				weaponInventory->addWeapon(WeaponType::TRIPMINE, 1);
			else if (pickup->getType() == WeaponType::RAPID_SHOT)
				weaponInventory->addWeapon(WeaponType::RAPID_SHOT, 5);

			weaponInventory->setCurrentWeapon(pickup->getType());
			player->setWeaponTexture(pickup->getType());
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

	// Check enemy
	bool allEnemiesDead = true;
	for (auto&e : enemies)
	{
		if (e->isAlive())
		{
			allEnemiesDead = false;
			break;
		}
	}

	// Reset wave
	if (allEnemiesDead && !waveManager->getWaveIntro() && !enemies.empty())
	{
		waveManager->nextWave();
		waveManager->startWaveIntro();

		for (auto& e : enemies) delete e;
		enemies.clear();

		if (enemyDirection == -1) enemyDirection = 1;
		moveInterval = 700;
	}

	// New Wave
	if (waveManager->getWaveIntro())
	{
		int rows = 5;
		int cols = 11;
		int spacingX = 13;
		int spacingY = 13;

		if (SDL_GetTicks() - waveManager->getWaveIntroStartTime() >= waveManager->getWaveIntroDuration())
		{
			waveManager->setShowingWaveIntro();
			enemies = Enemy::createFormation(renderer, rows, cols, spacingX, spacingY);
			comboManager->reset();
			bullet->deactivate();
		}
	}

	// Enemy bullets cleanup
	enemyBullets.erase(
		std::remove_if(enemyBullets.begin(), enemyBullets.end(),
			[](Bullet* b) 
			{
				if (!b->isActive()) 
				{
					delete b;
					return true;
				}
				return false;
			}),
		enemyBullets.end()
	);

	// Enemies cleanup
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(),
			[](Enemy* e) 
			{
				if (e->enemyIsDying() && e->enemyIsFinishedDeathAnimation()) 
				{
					delete e;
					return true;
				}
				return false;
			}),
		enemies.end()
	);

	//Pickups cleanup
	pickups.erase(
		std::remove_if(pickups.begin(), pickups.end(),
			[](Pickup* p) 
			{
				if (p->isCollected()) 
				{
					delete p;
					return true;
				}
				return false;
			}),
		pickups.end());

	player->update();
	bullet->update();
	bulletManager->update();
	comboManager->update();
	weaponInventory->update();
	player->setWeaponTexture(weaponInventory->getCurrentWeapon());
}

// ADD
void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	if (menuManager->isInMainMenu())
	{
		menuManager->render();
		return;
	}
	player->render();
	bullet->render();
	if(ufo->isActive() || ufo->UFOIsDying()) ufo->render();
	scoreManager->render(renderer);
	bulletManager->render();
	comboManager->render(renderer);
	weaponInventory->renderWeaponHUD(renderer);

	for (auto& enemy : enemies) 
	{
		if (enemy->isAlive() || enemy->enemyIsDying()) enemy->render();
	}

	for (auto& pickup : pickups) { pickup->render(); }
	for (Bullet* b : enemyBullets) { if (b->isActive()) b->render(); }

	if (waveManager->getWaveIntro())
		waveManager->showWaveIntro(renderer);

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
	unsigned int frameStart;
	int frameTime;

	while (isRunning)
	{
		deltaTime = calculateDeltaTime();
		frameStart = SDL_GetTicks();

		handleEvents();
		weaponInventory->updateDeltaTime(deltaTime);
		render();
		update();
		frameTime = SDL_GetTicks() - frameStart;
		if (FRAME_DELAY > frameTime)
		{
			SDL_Delay(FRAME_DELAY - frameTime);
		}
	}
}

// DELETE
void Game::clean() 
{
	delete player;
	delete bullet;
	delete enemy;
	delete ufo;
	delete comboManager;
	delete waveManager;
	delete weaponInventory;
	delete scoreManager;
	delete bulletManager;
	delete menuManager;
	for (auto& enemy : enemies) delete enemy;
	enemies.clear();
	for (auto& b : enemyBullets) delete b;
	enemyBullets.clear();
	for (auto& pickup : pickups) delete pickup;
	pickups.clear();
	TTF_Quit();
	IMG_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}