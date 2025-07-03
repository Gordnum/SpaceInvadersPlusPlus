#include "ScoreManager.h"

ScoreManager::ScoreManager(SDL_Renderer* renderer, const std::string& fontPath, int fontSize)
			 :score(0), font(nullptr), texture(nullptr), color({255, 255, 255, 255})
{
	font = TTF_OpenFont(fontPath.c_str(), fontSize);
	if(!font)
		SDL_Log("Failed to load font: %s", TTF_GetError());

	updateTexture(renderer);
}

ScoreManager::~ScoreManager()
{
	if(texture) SDL_DestroyTexture(texture);
	if(font) TTF_CloseFont(font);
}

void ScoreManager::addPoints(int amount){ score += amount;}

int ScoreManager::getScore() const { return score; }

void ScoreManager::updateTexture(SDL_Renderer* renderer)
{
	if(texture) SDL_DestroyTexture(texture);

	std::string scoreText = "Score: " + std::to_string(score);
	SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);

	textRect = { 20, 20, surface->w, surface->h };
	SDL_FreeSurface(surface);
}

void ScoreManager::render(SDL_Renderer* renderer)
{
	updateTexture(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, &textRect);
}