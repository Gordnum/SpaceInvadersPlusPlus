#pragma once
#include <SDL.h>
#include <vector>
#include <string>
#include <SDL_image.h>
#include "Utils.h"

class UFO
{
	private:
		float speed;
		bool active;
		SDL_Rect rect;
		SDL_Renderer* renderer;

		static std::vector<SDL_Texture*> deathTextures;
		static SDL_Texture* textures;
		bool dying = false;
		float deathTimer = 0.0f;
		float deathFrameDuration = 0.25f;
		int deathFrameIndex = 0;
		bool finishedDeathAnimation = false;
		
		int hoverChannel = -1;

	public:
		UFO(SDL_Renderer* renderer);
		void update(float deltaTime);
		void render();
		void reset();
		void activate();
		void deactivate();
		void startDeathAnimation();
		static void LoadTextures(SDL_Renderer* renderer);
		static void FreeTextures();
		bool isActive() const;
		int getX() const;
		int getY() const;
		SDL_Rect getRect() const;

		bool UFOIsDying() const { return dying; }
		bool UFOIsFinishedDeathAnimation() const { return finishedDeathAnimation; }
};