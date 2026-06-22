#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "Utils.h"

class ScorePopup
{
	private:
		SDL_Texture* texture;
		SDL_Rect rect;
		float lifetime;
		const float duration = 1.2f;
		const float speedY = 40.0f;

	public:
		ScorePopup(SDL_Renderer* renderer, const std::string& text, int x, int y, 
				   float multiplier, SDL_Color* overrideColor = nullptr);
		~ScorePopup();
		void update(float dt);
		void render(SDL_Renderer* renderer);
		bool isAlive() const;
};