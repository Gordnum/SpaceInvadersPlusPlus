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
	SDL_Rect scoreLabelRect;
	SDL_Rect scoreNumberRect;
	SDL_Texture* scoreLabelTexture;
	SDL_Texture* scoreNumberTexture;

	SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, SDL_Rect& outRect, int y, bool centerX = false, int fixedX = 0);

	void updateTexture(SDL_Renderer* renderer);

public:
	ScoreManager(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
	~ScoreManager();

	void addPoints(int amount);
	void render(SDL_Renderer* renderer);
	int getScore() const;
};