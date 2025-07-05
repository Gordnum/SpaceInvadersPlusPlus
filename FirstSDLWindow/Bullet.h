#pragma once
#include <SDL.h>

class Bullet
{
private:
	SDL_Rect rect;
	SDL_Renderer* renderer;
	bool active;

public:
	Bullet(SDL_Renderer* renderer);
	void fire(int x, int y);
	void fireFrom(int x, int y); // Declare
	void update();
	void render();
	SDL_Rect getRect() const;
	bool isActive() const;
	void deactivate();
};