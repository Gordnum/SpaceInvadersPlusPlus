#pragma once
#include <SDL.h>
#include <vector>
#include <map>
#include <string>
#include <SDL_image.h>

enum class EnemyType 
{ 
	CRAB, 
	OCTOPUS, 
	SQUID 
};

class Enemy
{
private:
	SDL_Rect rect;
	SDL_Renderer* renderer;
	bool alive;
	int rowIndex;
	EnemyType enemyType;
	int animationFrame;
	float animationTimer = 0.0f;
	float animationSpeed = 0.07f;
	static std::map<EnemyType, std::vector<SDL_Texture*>> textures;

public:
	Enemy(SDL_Renderer* renderer, EnemyType enemyType = EnemyType::CRAB);
	~Enemy();
	SDL_Rect getRect() const;
	void update(float deltaTime);
	void render();
	void destroy();
	bool isAlive() const;
	void setPosition(int x, int y);
	void move(int dx, int dy);
	void advanceAnimation();
	void setRowIndex(int row) { rowIndex = row; }
	bool isInTopRow() const { return rowIndex == 0; }
	EnemyType getType() const;

	static void LoadTextures(SDL_Renderer* renderer);
	static void FreeTextures();

	static std::vector<Enemy*> createFormation(SDL_Renderer* renderer, int rows, int cols, int spacingX = 10, int spacingY = 10);
};