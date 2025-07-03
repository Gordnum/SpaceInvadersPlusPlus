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

	public:
	Player(SDL_Renderer* renderer);
	void handleEvent(const SDL_Event& e);
	void render();
	void update();
	int getX() const;
	int getY() const;
};