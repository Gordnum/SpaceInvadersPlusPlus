#pragma once
#include <SDL.h>

class UFO
{
	private:
		int speed;
		bool active;
		SDL_Rect rect;
		SDL_Renderer* renderer;
		
	public:
		UFO(SDL_Renderer* renderer);
		void update();
		void render();
		void reset();
		void activate();
		void deactivate();
		bool isActive() const;
		int getX() const;
		int getY() const;
		SDL_Rect getRect() const;
};