#pragma once
#include <SDL.h>
#include <vector>

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

public:
	Enemy(SDL_Renderer* renderer, EnemyType enemyType = EnemyType::CRAB);
	SDL_Rect getRect() const;
	void update();
	void render();
	void destroy();
	bool isAlive() const;
	void setPosition(int x, int y);
	void move(int dx, int dy);
	void setRowIndex(int row) { rowIndex = row; }
	bool isInTopRow() const { return rowIndex == 0; }
	EnemyType getType() const;

	static std::vector<Enemy*> createFormation(SDL_Renderer* renderer, int rows, int cols, int spacingX = 10, int spacingY = 10);
};