#include "Game.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

constexpr int PLAYFIELD_UPPER_MARGIN = 75;
constexpr int PLAYFIELD_BOTTOM_MARGIN = 90;
constexpr int BORDER_THICKNESS = 4;
constexpr int PLAYFIELD_TOP_Y = PLAYFIELD_UPPER_MARGIN + BORDER_THICKNESS;
constexpr int PLAYFIELD_BOTTOM_Y = SCREEN_HEIGHT - PLAYFIELD_BOTTOM_MARGIN;

Game::Game()
	:window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), bullet(nullptr), enemy(nullptr), ufo(nullptr), boss(nullptr),
	scoreManager(nullptr), weaponInventory(nullptr), comboManager(nullptr), waveManager(nullptr), menuManager(nullptr),
	bulletManager(nullptr), isGameOver(false), gameOverStartTime(0) {
}


Game::~Game() { clean(); }

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

	SoundManager::init();

	window = SDL_CreateWindow("SpaceInvaders++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	menuManager = std::make_unique<MenuManager>(renderer);
	waveManager = std::make_unique<WaveManager>();
	UFO::LoadTextures(renderer);
	Enemy::LoadTextures(renderer);
	player = std::make_unique<Player>(renderer);
	player->loadWeaponTextures(renderer);
	bullet = std::make_unique<Bullet>(renderer);
	ufo = std::make_unique<UFO>(renderer);
	boss = std::make_unique<Boss>(renderer);
	scoreManager = std::make_unique<ScoreManager>(renderer);
	comboManager = std::make_unique<ComboManager>();
	weaponInventory = std::make_unique<WeaponInventory>();
	bulletManager = std::make_unique<BulletManager>(renderer);
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
				lastUFOSpawnTime = SDL_GetTicks();
				pickups.push_back(std::move(std::make_unique<Pickup>(renderer, 400, PLAYFIELD_UPPER_MARGIN, WeaponType::BOMB_SHOT)));
			}
			else if (startEndless)
			{
				currentMode = GameMode::ENDLESS;
				menuManager->setInMainMenu(false);
				lastUFOSpawnTime = SDL_GetTicks();
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
				if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_e)
					weaponInventory->startWeaponSwapAnimation(1);

				else if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_q)
					weaponInventory->startWeaponSwapAnimation(-1);
			}

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE && !bullet->isActive())
			{
				WeaponType currentWeapon = weaponInventory->getCurrentWeapon();
				int ammo = weaponInventory->getAmmo(currentWeapon);

				if (currentWeapon == WeaponType::DEFAULT || ammo > 0)
				{
					if (bulletManager->fire(player->getX(), player->getY(), currentWeapon, ammo))
					{
						SoundManager::playSound(weaponTypeToSoundID(currentWeapon));

						if (currentWeapon == WeaponType::PIERCING_SHOT)
						{
							flashActive = true;
							flashStartTime = SDL_GetTicks();
						}

						if (currentWeapon != WeaponType::DEFAULT)
							weaponInventory->useAmmo(currentWeapon);
					}
				}
			}
		}

		player->handleEvent(event);
	}
}

// ADD
void Game::update()
{
	if (menuManager->isInMainMenu())
	{
		if (ufo->isActive())
		{
			ufo->reset();
			ufo->deactivate();
			lastUFOSpawnTime = SDL_GetTicks();
		}

		if (boss->isActive()) boss->deactivate();

		return;
	}

	if (isGameOver)
	{
		unsigned int now = SDL_GetTicks();

		if (now - gameOverStartTime >= 5000)
		{
			enemies.clear();
			enemyBullets.clear();
			bossBullets.clear();
			pickups.clear();
			bulletManager->clear();
			rowLastShotTime.clear();
			rowHasActiveBullet.clear();

			boss->deactivate();
			ufo->deactivate();
			lastUFOSpawnTime = SDL_GetTicks();

			waveManager->reset();
			comboManager->reset();
			weaponInventory->reset();
			scoreManager->reset();
			player->reset();

			highScoreSaved = false;
			isGameOver = false;

			menuManager->setInMainMenu(true);
			moveInterval = 700;
			if (enemyDirection == -1) enemyDirection = 1;

			return;
		}

		return;
	}

	// FPS Counter
	frameCount++;
	unsigned currentTicks = SDL_GetTicks();
	if (currentTicks - fpsTimer >= 1000)
	{
		std::cout << "FPS: " << frameCount << std::endl;
		frameCount = 0;
		fpsTimer = currentTicks;
	}

	bool inWaveIntro = waveManager->getWaveIntro();

	if (inWaveIntro)
	{
		// Still allow the intro timer to move
		if (SDL_GetTicks() - waveManager->getWaveIntroStartTime() >= waveManager->getWaveIntroDuration())
		{
			waveManager->setShowingWaveIntro(false);

			// Spawn enemies if not in boss intro
			if (!boss->isActive())
			{
				int rows = (waveManager->getWave() >= 11) ? 6 : 5;
				int cols = 11;
				int spacingX = 13;
				int spacingY = 13;

				enemies = Enemy::createFormation(renderer, rows, cols, spacingX, spacingY);
				comboManager->reset();
				bullet->deactivate();
			}
		}
	}

	// Boss update + bullet hit boss
	if (boss->isActive() && !waveManager->getWaveIntro())
	{
		boss->update(deltaTime, renderer, bossBullets);

		for (auto& b : bulletManager->getBullets())
		{
			if (b->isActive() && checkCollision(b->getRect(), boss->getRect()))
			{
				if (b->getCurrentWeapon() == WeaponType::TRIPMINE)
				{
					pendingTripmines.push_back
					(
						{
							boss->getRect().y + 20,     // rowY
							SDL_GetTicks() + 700,       // delay
							0,                          // renderUntil
							false,                      // exploded
							true                        // hitBoss
						}
					);

					b->deactivate();
					SoundManager::playSound(SoundID::TRIPMINE_SET_AND_EXPLODE);
					continue;
				}

				int damage = 1;

				// You can modify damage based on weapon type
				switch (b->getCurrentWeapon())
				{
				case WeaponType::DEFAULT:
				case WeaponType::RAPID_SHOT:
					damage = 10;
					break;
				case WeaponType::PIERCING_SHOT:
					damage = 20;
					break;
				case WeaponType::BOMB_SHOT:
					damage = 30;
					break;
				}

				boss->takeDamage(damage);
				SoundManager::playSound(SoundID::BOSS_TAKE_DAMAGE);

				if (b->getCurrentWeapon() != WeaponType::PIERCING_SHOT)
					b->deactivate();

				if (boss->isDefeated() && currentMode == GameMode::CAMPAIGN)
				{
					boss->deactivate();
					waveManager->setShowingWaveIntro(false);

					// Win logic / trigger end screen (prototype)
					isGameOver = true;
					gameOverStartTime = SDL_GetTicks();
				}
			}
		}
	}

	// Boss spawn enemy
	if (boss->isActive() && !waveManager->getWaveIntro() && boss->shouldSpawnEnemies())
	{
		EnemyType type;
		int typeRoll = rand() % 3;
		switch (typeRoll)
		{
		case 0: type = EnemyType::CRAB; break;
		case 1: type = EnemyType::OCTOPUS; break;
		case 2: type = EnemyType::SQUID; break;
		}

		int enemyAmount = 2 + rand() % 3; // 2 to 4 enemies

		// Direction and start position
		bool fromLeft = (rand() % 2) == 0;
		int startX = fromLeft ? 70 : SCREEN_WIDTH - 100;
		if (enemyAmount > 2 && fromLeft)
			startX = 150;
		else if (enemyAmount > 2 && !fromLeft)
			startX = SCREEN_WIDTH - 200;

		int y = 175 + rand() % 150;
		int speed = fromLeft ? 20 : -20;

		for (int i = 0; i < enemyAmount; ++i)
		{
			auto enemy = std::make_unique<Enemy>(renderer, type);

			SDL_Rect r = enemy->getRect();
			if (fromLeft)
				r.x = startX - i * (r.w + 13); // If starting off-screen left, space them leftward
			else
				r.x = startX + i * (r.w + 13); // If starting off-screen right, space them rightward

			r.y = y;
			enemy->setRect(r);

			enemy->setOrigin(EnemyOrigin::BOSS_SPAWNED);
			enemy->setEnemyBossSpeed(speed);

			enemies.push_back(std::move(enemy));
		}
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
			if (enemy->getOrigin() == EnemyOrigin::BOSS_SPAWNED) continue;

			SDL_Rect r = enemy->getRect();
			int nextX = r.x + enemyDirection * static_cast<int>(waveManager->getEnemySpeed());

			if (nextX <= 0 || (nextX + r.w) >= SCREEN_WIDTH)
			{
				willBounce = true;
				break;
			}
		}

		if (willBounce)
		{
			enemyDirection *= -1;

			for (auto& enemy : enemies)
			{
				if (!enemy->isAlive()) continue;
				if (enemy->getOrigin() != EnemyOrigin::NORMAL) continue;

				enemy->move(0, 15);
				enemy->advanceAnimation();
			}

			if (moveInterval > 100)
				moveInterval -= 60;

			std::cout << "DROP\n";

			lastMoveTime = currentTime;
			return;
		}

		for (auto& enemy : enemies)
		{
			if (enemy->isAlive() && enemy->getOrigin() == EnemyOrigin::NORMAL)
			{
				enemy->move(static_cast<int>(enemyDirection * waveManager->getEnemySpeed()), 0);
				enemy->advanceAnimation();
			}
			else if (enemy->isAlive() && enemy->getOrigin() == EnemyOrigin::BOSS_SPAWNED)
			{
				enemy->move(enemy->getEnemyBossSpeed(), 0);
				enemy->advanceAnimation();
			}
		}
		lastMoveTime = currentTime;
	}

	// Enemy shoot
	unsigned int now = SDL_GetTicks();

	std::unordered_map<int, std::vector<Enemy*>> rowEnemies;

	for (const auto& e : enemies)
	{
		if (!e->isAlive()) continue;
		if (e->getType() != EnemyType::SQUID) continue;

		rowEnemies[e->getRowIndex()].push_back(e.get());
	}

	for (auto& pair : rowEnemies)
	{
		int row = pair.first;

		if (rowLastShotTime.find(row) == rowLastShotTime.end())
		{
			rowLastShotTime[row] = 0;
			rowHasActiveBullet[row] = false;
		}

		if (rowHasActiveBullet[row])
			continue;

		if (now - rowLastShotTime[row] < enemyShootCooldown)
			continue;

		auto& shooters = pair.second;
		if (shooters.empty())
			continue;

		Enemy* shooter = shooters[rand() % shooters.size()];

		auto bullet = std::make_unique<Bullet>(renderer);

		SDL_Rect r = shooter->getRect();
		bullet->fireFrom(r.x + r.w / 2, r.y + r.h, true);
		bullet->setEnemyBulletSpeed(waveManager->getProjectileSpeed());
		bullet->setRowIndex(row);

		enemyBullets.push_back(std::move(bullet));

		rowLastShotTime[row] = now;
		rowHasActiveBullet[row] = true;

		SoundManager::playSound(SoundID::ENEMY_SHOOT);
	}

	// bullet hit enemy
	for (auto& b : bulletManager->getBullets())
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
					scoreManager->awardScore(earnedScore, *player);
					comboManager->onEnemyKilled();
					SoundManager::playSound(SoundID::ENEMY_DEATH);
				}
				else if (b->getCurrentWeapon() == WeaponType::BOMB_SHOT)
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
								scoreManager->awardScore(earnedScore, *player);
								comboManager->onEnemyKilled();
							}
						}
					}
					SoundManager::playSound(SoundID::ENEMY_DEATH);
					SoundManager::playSound(SoundID::BOMB_EXPLODE);
					b->deactivate();
				}
				else if (b->getCurrentWeapon() == WeaponType::TRIPMINE)
				{
					pendingTripmines.push_back(
						{
							enemy->getRect().y,          // rowY
							SDL_GetTicks() + 700,         // trigger time
							false,                        // exploded or not
							0                             // renderUntil
						}
					);

					b->deactivate(); // remove tripmine bullet after explosion
					SoundManager::playSound(SoundID::TRIPMINE_SET_AND_EXPLODE);
				}
				else if (b->getCurrentWeapon() == WeaponType::DEFAULT || b->getCurrentWeapon() == WeaponType::RAPID_SHOT)
				{
					b->deactivate();
					enemy->destroy();
					int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
					scoreManager->awardScore(earnedScore, *player);
					comboManager->onEnemyKilled();
					SoundManager::playSound(SoundID::ENEMY_DEATH);
				}

				if (scoreManager->spawnPickup())
				{
					auto randomWeapons = weaponInventory->randomizeWeapon();
					if (randomWeapons.empty())
						continue;

					int index = rand() % randomWeapons.size();
					WeaponType randomWeapon = randomWeapons[index];

					int randomX = 50 + (rand() % (SCREEN_WIDTH - 100));
					pickups.push_back(std::make_unique<Pickup>(renderer, randomX, PLAYFIELD_UPPER_MARGIN, randomWeapon));
				}
			}
		}
	}

	// TRIPMINE delay explosion
	unsigned int explodeNow = SDL_GetTicks();
	const int ROW_THRESHOLD = 10;

	for (auto& explosion : pendingTripmines)
	{
		if (explosion.exploded) continue;
		if (explodeNow < explosion.triggerTime) continue;

		for (auto& enemy : enemies)
		{
			if (!enemy->isAlive()) continue;

			int enemyY = enemy->getRect().y;
			if (std::abs(enemyY - explosion.rowY) < ROW_THRESHOLD)
			{
				enemy->destroy();

				int baseScore = 30;
				int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
				scoreManager->awardScore(earnedScore, *player);
				comboManager->onEnemyKilled();

				if (scoreManager->spawnPickup())
				{
					auto weapons = weaponInventory->randomizeWeapon();
					if (!weapons.empty())
					{
						WeaponType w = weapons[rand() % weapons.size()];
						int x = 50 + rand() % (SCREEN_WIDTH - 100);
						pickups.push_back(std::make_unique<Pickup>(renderer, x, PLAYFIELD_UPPER_MARGIN, w));
					}
				}
				SoundManager::playSound(SoundID::ENEMY_DEATH);
			}
		}

		if (explosion.hitBoss && boss->isActive())
		{
			const int TRIPMINE_BOSS_DAMAGE = 30;
			boss->takeDamage(TRIPMINE_BOSS_DAMAGE);
			SoundManager::playSound(SoundID::BOSS_TAKE_DAMAGE);
		}

		explosion.exploded = true;
		explosion.renderUntil = SDL_GetTicks() + 50; // how long the tripmine line stays
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

	// if hit with boss bullets
	for (auto& b : bossBullets)
	{
		if (b->isActive() && checkCollision(b->getRect(), player->getRect()))
		{
			b->deactivate();
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
	for (auto& bullet : enemyBullets)
	{
		if (bullet->isActive()) bullet->update(deltaTime);
	}

	// update boss bullet
	for (auto& b : bossBullets)
	{
		if (b->isActive()) b->update(deltaTime);
	}

	// bullet hit on ufo
	for (auto& b : bulletManager->getBullets())
	{
		if (b->isActive() && ufo->isActive() &&
			checkCollision(b->getRect(), ufo->getRect()))
		{
			if (b->getCurrentWeapon() == WeaponType::PIERCING_SHOT)
			{
				ufo->startDeathAnimation();
				ufo->deactivate();
			}
			else if (b->getCurrentWeapon() == WeaponType::BOMB_SHOT)
			{
				b->deactivate();
				ufo->startDeathAnimation();
				ufo->deactivate();
			}
			else if (b->getCurrentWeapon() == WeaponType::TRIPMINE)
			{
				b->deactivate();
				ufo->startDeathAnimation();
				ufo->deactivate();
			}
			else if (b->getCurrentWeapon() == WeaponType::DEFAULT || b->getCurrentWeapon() == WeaponType::RAPID_SHOT)
			{
				b->deactivate();
				ufo->startDeathAnimation();
				ufo->deactivate();
			}

			int baseScore = 200;
			int earnedScore = static_cast<int>(baseScore * comboManager->getMultiplier());
			scoreManager->awardScore(earnedScore, *player);
			comboManager->onEnemyKilled();

			int index = rand() % weaponInventory->randomizeWeapon().size();
			WeaponType randomWeapon = weaponInventory->randomizeWeapon()[index];

			int randomX = 50 + (rand() % (SCREEN_WIDTH - 100)); // safe margin
			pickups.push_back(std::move(std::make_unique<Pickup>(renderer, ufo->getX(), ufo->getY(), randomWeapon)));
			if (scoreManager->spawnPickup())
				pickups.push_back(std::move(std::make_unique<Pickup>(renderer, randomX, PLAYFIELD_UPPER_MARGIN, randomWeapon)));
			if (scoreManager->giveLive())
				player->plusLives();
		}
	}

	// UFO
	if (!boss->isActive() && currentTime - lastUFOSpawnTime >= ufoSpawnInterval)
	{
		if (!ufo->isActive())
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
			SoundManager::playSound(SoundID::PICKUP_COLLECT);
		}
		pickup->update(deltaTime);
	}

	// NEW HIGHSCORE
	if (isGameOver && !highScoreSaved)
	{
		scoreManager->saveHighScore("highscore.txt");
		highScoreSaved = true;
	}

	// Check enemy status
	bool allEnemiesDead = true;
	for (auto& e : enemies)
	{
		if (e->getOrigin() == EnemyOrigin::BOSS_SPAWNED) continue;

		if (e->isAlive())
		{
			allEnemiesDead = false;
			break;
		}
	}

	// Reset wave
	if (allEnemiesDead && !waveManager->getWaveIntro() && !enemies.empty() && !boss->isActive())
	{
		waveManager->nextWave();

		if (currentMode == GameMode::CAMPAIGN && waveManager->getWave() == 20)
		{
			boss->activate();
			ufo->deactivate();
			enemies.clear();
			waveManager->startWaveIntro(waveIntroType::BOSS);
		}
		else
		{
			waveManager->startWaveIntro();
			enemies.clear();

			if (enemyDirection == -1) enemyDirection = 1;
			moveInterval = 700;
		}
	}

	// New Wave
	if (waveManager->getWaveIntro() && !boss->isActive())
	{
		int rows = (waveManager->getWave() >= 11) ? 6 : 5;
		int cols = 11;
		int spacingX = 13;
		int spacingY = 13;

		if (SDL_GetTicks() - waveManager->getWaveIntroStartTime() >= waveManager->getWaveIntroDuration())
		{
			waveManager->setShowingWaveIntro(false);
			enemies = Enemy::createFormation(renderer, rows, cols, spacingX, spacingY);
			comboManager->reset();
			bullet->deactivate();
		}
	}

	//enemy bullets row cleanup
	for (auto& b : enemyBullets)
	{
		if (!b->isActive())
		{
			int row = b->getRowIndex();
			rowHasActiveBullet[row] = false;
		}
	}

	// Enemy bullets cleanup
	enemyBullets.erase(
		std::remove_if(enemyBullets.begin(), enemyBullets.end(),
			[](const std::unique_ptr<Bullet>& b)
			{
				return !b->isActive();
			}),
		enemyBullets.end()
	);

	// Enemies cleanup
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(),
			[](const std::unique_ptr<Enemy>& e)
			{
				return e->enemyIsDying() && e->enemyIsFinishedDeathAnimation();
			}),
		enemies.end());

	//Pickups cleanup
	pickups.erase(
		std::remove_if(pickups.begin(), pickups.end(),
			[](const std::unique_ptr<Pickup>& p)
			{
				return p->isCollected();
			}),
		pickups.end());

	//Boss bullets cleanup
	bossBullets.erase(
		std::remove_if(bossBullets.begin(), bossBullets.end(),
			[](const std::unique_ptr<BossBullet>& b)
			{
				return !b->isActive();
			}),
		bossBullets.end());

	//TRIPMINE delay cleanup
	pendingTripmines.erase(
		std::remove_if(pendingTripmines.begin(), pendingTripmines.end(),
			[](const TripmineExplosion& e)
			{
				return e.exploded && SDL_GetTicks() > e.renderUntil;
			}),
		pendingTripmines.end()
	);

	player->update(deltaTime);
	for (auto& enemy : enemies) { enemy->update(deltaTime); }
	ufo->update(deltaTime);
	bullet->update(deltaTime);
	bulletManager->update(deltaTime);
	comboManager->update(deltaTime);
	weaponInventory->update(deltaTime);
	player->setWeaponTexture(weaponInventory->getCurrentWeapon());
}

// ADD
void Game::render()
{
	bool inWaveIntro = waveManager->getWaveIntro();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//game border
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_Rect topBorder
	{
		0,
		PLAYFIELD_TOP_Y - BORDER_THICKNESS,
		SCREEN_WIDTH,
		BORDER_THICKNESS
	};

	SDL_Rect bottomBorder
	{
		0,
		PLAYFIELD_BOTTOM_Y,
		SCREEN_WIDTH,
		BORDER_THICKNESS
	};

	SDL_RenderFillRect(renderer, &topBorder);
	SDL_RenderFillRect(renderer, &bottomBorder);

	//clip game objects
	SDL_Rect playfieldClip
	{
		0,
		PLAYFIELD_TOP_Y,
		SCREEN_WIDTH,
		PLAYFIELD_BOTTOM_Y - PLAYFIELD_TOP_Y
	};

	SDL_RenderSetClipRect(renderer, &playfieldClip);

	if (menuManager->isInMainMenu())
	{
		menuManager->render();
		return;
	}
	bullet->render();
	bulletManager->render();
	if (!inWaveIntro)
	{
		for (auto& enemy : enemies)
		{
			if (enemy->isAlive() || enemy->enemyIsDying())
				enemy->render();
		}

		if (boss->isActive()) boss->render(renderer);

		for (auto& b : enemyBullets)
		{
			if (b->isActive()) b->render();
		}


		for (auto& b : bossBullets)
		{
			if (b->isActive()) b->render(renderer);
		}
	}

	if (ufo->isActive() || ufo->UFOIsDying()) ufo->render();

	SDL_RenderSetClipRect(renderer, nullptr);

	player->render();
	scoreManager->render(renderer);
	comboManager->render(renderer);
	weaponInventory->renderWeaponHUD(renderer);

	for (auto& enemy : enemies) { if (enemy->isAlive() || enemy->enemyIsDying()) enemy->render(); }
	for (auto& pickup : pickups) { pickup->render(); }
	for (auto& b : enemyBullets) { if (b->isActive()) b->render(); }
	for (auto& b : bossBullets) { if (b->isActive()) b->render(renderer); }

	if (waveManager->getWaveIntro() && !isGameOver) //prototype game win
		waveManager->showWaveIntro(renderer);

	//render TRIPMINE exploding line
	unsigned int renderExplodingNow = SDL_GetTicks();
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // tripmine green

	for (const auto& explosion : pendingTripmines)
	{
		if (!explosion.exploded) continue;
		if (renderExplodingNow > explosion.renderUntil) continue;

		int y = explosion.rowY + 10; // center-ish of enemy row

		const int LINE_THICKNESS = 6;

		SDL_Rect horizontalBeam
		{
			0,
			y - LINE_THICKNESS / 2,
			SCREEN_WIDTH,
			LINE_THICKNESS
		};

		SDL_RenderFillRect(renderer, &horizontalBeam);
	}

	//render laser pointer for piercing
	if (weaponInventory->getCurrentWeapon() == WeaponType::PIERCING_SHOT)
	{
		auto renderPiercingLaser = [&](const SDL_Rect& p)
			{
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

				int x = p.x + p.w / 2;
				SDL_Rect laserPointer
				{
					x - 1,
					PLAYFIELD_TOP_Y,
					3,
					(p.y + 4) - PLAYFIELD_TOP_Y
				};

				SDL_RenderFillRect(renderer, &laserPointer);
			};

		renderPiercingLaser(player->getRect());
	}

	// White flash effect
	if (flashActive)
	{
		unsigned int flashNow = SDL_GetTicks();
		unsigned int flashElapsed = flashNow - flashStartTime;

		if (flashElapsed >= flashDuration)
		{
			flashActive = false;
		}
		else
		{
			float t = 1.0f - (float)flashElapsed / flashDuration;
			Uint8 alpha = static_cast<Uint8>(255 * t);

			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);

			SDL_Rect screen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
			SDL_RenderFillRect(renderer, &screen);
		}
	}

	//render game over
	if (isGameOver)
	{
		unsigned int gameOverScreen = SDL_GetTicks();

		if (gameOverScreen - gameOverStartTime >= 2000) // time to activate the game over screen
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
		render();
		update();
		frameTime = SDL_GetTicks() - frameStart;
		if (FRAME_DELAY > frameTime)
		{
			SDL_Delay(FRAME_DELAY - frameTime);
		}
	}
}

void Game::clean()
{
	SoundManager::clean();
	enemies.clear();
	enemyBullets.clear();
	pickups.clear();
	bulletManager->clear();
	bossBullets.clear();

	menuManager.reset();
	waveManager.reset();
	player.reset();
	bullet.reset();
	ufo.reset();
	boss.reset();
	scoreManager.reset();
	comboManager.reset();
	weaponInventory.reset();
	bulletManager.reset();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}