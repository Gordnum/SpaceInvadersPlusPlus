#pragma once
#include <SDL.h>

class Player
{
	private:
	SDL_Rect rect;
	SDL_Renderer* renderer;
	int speed;
	bool movingLeft;
	bool movingRight;
	int playerLives;

	public:
	Player(SDL_Renderer* renderer);
	void handleEvent(const SDL_Event& e);
	void render();
	void update();
	void loseLives();
	int getX() const;
	int getY() const;
	int getLives() const;
	SDL_Rect getRect() const;
};