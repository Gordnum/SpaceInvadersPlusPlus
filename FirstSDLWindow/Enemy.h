#pragma once
#include <SDL.h>
#include <vector>
#include <map>
#include <string>
#include <SDL_image.h>
#include <iostream>
#include <memory>

enum class EnemyType 
{ 
	CRAB, 
	OCTOPUS, 
	SQUID 
};

enum class EnemyOrigin
{
	NORMAL,
	BOSS_SPAWNED
};

class Enemy
{
	private:
		SDL_Rect rect;
		SDL_Renderer* renderer;
		bool alive;
		int rowIndex;
		EnemyType enemyType;
		EnemyOrigin origin = EnemyOrigin::NORMAL;
		int animationFrame;
		static std::map<EnemyType, std::vector<SDL_Texture*>> textures;
		static std::vector<SDL_Texture*> deathTextures;
		bool dying = false;
		float deathTimer = 0.0f;
		float deathFrameDuration = 0.08f;
		int deathFrameIndex = 0;
		bool finishedDeathAnimation = false;
		int enemyBossSpeed = 0;

	public:
		Enemy(SDL_Renderer* renderer, EnemyType enemyType = EnemyType::OCTOPUS);
		SDL_Rect getRect() const;
		void update(float deltaTime);
		void render();
		void destroy();
		bool isAlive() const;
		void setPosition(int x, int y);
		void move(int dx, int dy);
		void advanceAnimation();
		void setRowIndex(int row) { rowIndex = row; }
		int getRowIndex() const { return rowIndex; };
		bool isInTopRow() const { return rowIndex == 0; }
		void setOrigin(EnemyOrigin o);
		EnemyOrigin getOrigin() const;
		void setRect(SDL_Rect r);
		EnemyType getType() const;

		void setEnemyBossSpeed(int speed) { enemyBossSpeed = speed; }
		float getEnemyBossSpeed() const { return enemyBossSpeed; }

		static void LoadTextures(SDL_Renderer* renderer);
		static void FreeTextures();

		bool enemyIsDying() const { return dying; }
		bool enemyIsFinishedDeathAnimation() const { return finishedDeathAnimation; }

		int getX() const;
		int getY() const;

		static std::vector<std::unique_ptr<Enemy>> createFormation(SDL_Renderer* renderer, int rows, int cols, int spacingX, int spacingY);
};