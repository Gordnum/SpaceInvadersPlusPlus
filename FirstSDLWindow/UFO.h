#pragma once
#include <SDL.h>
#include <vector>
#include <string>
#include <SDL_image.h>

class UFO
{
	private:
		int speed;
		bool active;
		SDL_Rect rect;
		SDL_Renderer* renderer;

		static std::vector<SDL_Texture*> deathTextures;
		static SDL_Texture* textures;
		bool dying = false;
		float deathTimer = 0.0f;
		float deathFrameDuration = 0.02f;
		int deathFrameIndex = 0;
		bool finishedDeathAnimation = false;
		
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