#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class ScoreManager
{
	private:
		int score;
		TTF_Font* font;
		SDL_Color color;
		SDL_Rect textRect;
		SDL_Texture* texture;

		void updateTexture(SDL_Renderer* renderer);

	public:
		ScoreManager(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
		~ScoreManager();

		void addPoints(int amount);
		void render(SDL_Renderer* renderer);
		int getScore() const;
};