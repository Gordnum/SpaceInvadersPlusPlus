#include "ScoreManager.h"
#include "ComboManager.h"
#include <stdio.h>
#include <fstream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

ScoreManager::ScoreManager(SDL_Renderer* renderer)
	:score(0), font(nullptr), scoreLabelTexture(nullptr), color({ 255, 255, 255, 255 }), 
	 highscore(0), nextPickupThreshold(500), nextGiveLivesThreshold(1000), shouldSpawnPickup(false)
{
	font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 20);
	if (!font)
		SDL_Log("Failed to load font: %s", TTF_GetError());

	updateTexture(renderer);
}

ScoreManager::~ScoreManager()
{
	if (scoreLabelTexture) SDL_DestroyTexture(scoreLabelTexture);
	if (scoreNumberTexture) SDL_DestroyTexture(scoreNumberTexture);
	if (highScoreLabelTexture) SDL_DestroyTexture(highScoreLabelTexture);
	if (highScoreNumberTexture) SDL_DestroyTexture(highScoreNumberTexture);

	if (font) TTF_CloseFont(font);
}

void ScoreManager::addPoints(int amount) 
{
	printf("\t+ %d\n", amount);
	score += amount;

	if(score >= nextPickupThreshold)
	{
		shouldSpawnPickup = true;
		nextPickupThreshold += 500;
	}

	if(score >= nextGiveLivesThreshold)
	{
		shouldGiveLive = true;
		nextGiveLivesThreshold += 1000;
	}
}

int ScoreManager::getScore() const { return score; }

void ScoreManager::saveHighScore(const std::string& filename)
{
	std::ofstream file(filename);
	if (file.is_open()) 
	{
		file << highscore;
		file.close();
	}
}

void ScoreManager::loadHighScore(const std::string& filename)
{
	std::ifstream file(filename);
	if (file.is_open()) 
	{
		file >> highscore;
		file.close();
	}
}

SDL_Texture* ScoreManager::renderText(SDL_Renderer* renderer, const std::string& text, SDL_Rect& outRect, int y, bool centerX, int fixedX)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (!surface) return nullptr;

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);

	int x = centerX ? (SCREEN_WIDTH - surface->w) / 2 : fixedX;
	outRect = { x, y, surface->w, surface->h };

	SDL_FreeSurface(surface);
	return tex;
}

void ScoreManager::updateTexture(SDL_Renderer* renderer)
{
	if (score > highscore)
		highscore = score;

	// --- Render "Score" label ---
	if (scoreLabelTexture) SDL_DestroyTexture(scoreLabelTexture);
	scoreLabelTexture = renderText(renderer, "<SCORE>", scoreLabelRect, 20, false, 100); // left-aligned at x=100

	if(highScoreLabelTexture)  SDL_DestroyTexture(highScoreLabelTexture);
	highScoreLabelTexture = renderText(renderer, "<HISCORE>", highScoreLabelRect, 20, true, 100);

	// --- Render score number under the label ---
	if (scoreNumberTexture) SDL_DestroyTexture(scoreNumberTexture);
	if (highScoreNumberTexture) SDL_DestroyTexture(highScoreNumberTexture);

	// Render number to surface (just to get width)
	std::string scoreStr = std::to_string(score);
	SDL_Surface* numSurface = TTF_RenderText_Solid(font, scoreStr.c_str(), color);
	scoreNumberTexture = SDL_CreateTextureFromSurface(renderer, numSurface);

	std::string highScoreStr = std::to_string(highscore);
	SDL_Surface* highNumSurface = TTF_RenderText_Solid(font, highScoreStr.c_str(), color);
	highScoreNumberTexture = SDL_CreateTextureFromSurface(renderer, highNumSurface);

	// Compute center x of label, then shift number to center under it
	int centerX = scoreLabelRect.x + scoreLabelRect.w / 2;
	int numX = centerX - numSurface->w / 2;
	int numY = scoreLabelRect.y + scoreLabelRect.h;

	int highCenterX = highScoreLabelRect.x + highScoreLabelRect.w / 2;
	int highNumX = highCenterX - highNumSurface->w / 2;
	int highNumY = highScoreLabelRect.y + highScoreLabelRect.h;

	scoreNumberRect = { numX, numY, numSurface->w, numSurface->h };
	highScoreNumberRect = { highNumX, highNumY, highNumSurface->w, highNumSurface->h };
	SDL_FreeSurface(numSurface);
	SDL_FreeSurface(highNumSurface);
}

void ScoreManager::render(SDL_Renderer* renderer)
{
	updateTexture(renderer);
	if (scoreLabelTexture)
		SDL_RenderCopy(renderer, scoreLabelTexture, nullptr, &scoreLabelRect);
	if (scoreNumberTexture)
		SDL_RenderCopy(renderer, scoreNumberTexture, nullptr, &scoreNumberRect);
	if (highScoreLabelTexture)
		SDL_RenderCopy(renderer, highScoreLabelTexture, nullptr, &highScoreLabelRect);
	if (highScoreNumberTexture)
		SDL_RenderCopy(renderer, highScoreNumberTexture, nullptr, &highScoreNumberRect);
}

bool ScoreManager::spawnPickup() 
{ 
	if(shouldSpawnPickup)
	{
		shouldSpawnPickup = false;
		return true;
	}
	return false;
}

bool ScoreManager::giveLive()
{
	if(shouldGiveLive)
	{
		shouldGiveLive = false;
		return true;
	}
	return false;
}

