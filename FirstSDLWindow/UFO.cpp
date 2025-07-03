#include "UFO.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

UFO::UFO(SDL_Renderer* renderer)
	:renderer(renderer), active(false), speed(5)
{
	rect = { -60, 80, 60, 30 };
}

void UFO::update()
{
	if (!active) return;

	rect.x += speed;

	if (rect.x > SCREEN_WIDTH)
		deactivate();
}

void UFO::render()
{
	if(active)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
}

void UFO::reset()
{
	rect.x = -rect.w;
	active = true;
}

void UFO::activate(){ reset(); }

void UFO::deactivate() { active = false; }

bool UFO::isActive() const { return active; }

SDL_Rect UFO::getRect() const{return rect;}