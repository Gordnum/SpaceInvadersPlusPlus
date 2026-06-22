#include "ScoreManager.h"
#include "ComboManager.h"
#include <stdio.h>
#include <fstream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

ScoreManager::ScoreManager(SDL_Renderer* renderer)
	:score(0), font(nullptr), scoreLabelTexture(nullptr), color({ 255, 255, 255, 255 }),
	campaignHighscore(0), endlessHighscore(0), nextPickupThreshold(500), nextGiveLivesThreshold(5000), shouldSpawnPickup(false), 
	endlessUnlocked(false), currentMode(GameMode::CAMPAIGN)
{
	std::string base = getExeDir();

	font = TTF_OpenFont((base + "Assets\\Fonts\\space_invaders.ttf").c_str(), 20);
	if (!font)
	{
		SDL_Log("Failed to load font: %s", TTF_GetError());
		return;
	}

	TTF_SetFontStyle(font, TTF_STYLE_BOLD);

	updateTexture(renderer, currentMode);
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

	if (score >= nextPickupThreshold)
	{
		shouldSpawnPickup = true;
		nextPickupThreshold += 500;
	}

	if (score >= nextGiveLivesThreshold)
	{
		shouldGiveLive = true;
		nextGiveLivesThreshold += 5000;
	}
}

void ScoreManager::awardScore(int points)
{
	addPoints(points);
}

int ScoreManager::getScore() const { return score; }

int ScoreManager::getCampaignHighScore() const
{
	return campaignHighscore;
}

int ScoreManager::getEndlessHighScore() const
{
	return endlessHighscore;
}

void ScoreManager::setCampaignHighScore(int value)
{
	if (value > campaignHighscore)
	{
		campaignHighscore = value;
		saveHighScore((getExeDir() + "Assets\\highscore.txt").c_str());
	}
}

void ScoreManager::setEndlessHighScore(int value)
{
	if (value > endlessHighscore)
	{
		endlessHighscore = value;
		saveHighScore((getExeDir() + "Assets\\highscore.txt").c_str());
	}
}

void ScoreManager::saveHighScore(const std::string& filename)
{
	std::string plaintext = std::to_string(campaignHighscore) + "\n" +
							std::to_string(endlessHighscore) + "\n" +
							std::to_string(endlessUnlocked);
	
	std::string encrypted = xorEncryptDecrypt(plaintext);

	std::ofstream file(filename, std::ios::binary);
	if (file.is_open())
	{
		file.write(encrypted.c_str(), encrypted.size());
		file.close();
	}
}

void ScoreManager::loadHighScore(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) return;

	std::string encrypted((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	std::string decrypted = xorEncryptDecrypt(encrypted);

	try
	{
		std::istringstream ss(decrypted);
		ss >> campaignHighscore;
		ss >> endlessHighscore;
		if (!(ss >> endlessUnlocked))
			endlessUnlocked = false;
	}
	catch (...)
	{
		campaignHighscore = 0;
		endlessHighscore = 0;
		endlessUnlocked = false;
	}
}

void ScoreManager::reset()
{
	score = 0;
	nextPickupThreshold = 500;
	nextGiveLivesThreshold = 5000;
	shouldGiveLive = false;
	shouldSpawnPickup = false;
}

SDL_Texture* ScoreManager::renderText(SDL_Renderer* renderer, const std::string& text, SDL_Rect& outRect, int y, bool centerX, int fixedX)
{
	if (!font)
	{
		SDL_Log("Font not loaded; skipping renderText for '%s'", text.c_str());
		return nullptr;
	}

	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (!surface)
	{
		SDL_Log("Failed to render text: %s", TTF_GetError());
		return nullptr;
	}

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);

	int x = centerX ? (SCREEN_WIDTH - surface->w) / 2 : fixedX;
	outRect = { x, y, surface->w, surface->h };

	SDL_FreeSurface(surface);
	return tex;
}

void ScoreManager::updateTexture(SDL_Renderer* renderer, GameMode mode)
{
	// Render "Score" label
	if (scoreLabelTexture) SDL_DestroyTexture(scoreLabelTexture);
	scoreLabelTexture = renderText(renderer, "<SCORE>", scoreLabelRect, 20, false, 100); // left-aligned at x=100

	if (highScoreLabelTexture)  SDL_DestroyTexture(highScoreLabelTexture);
	highScoreLabelTexture = renderText(renderer, "<HI-SCORE>", highScoreLabelRect, 20, true, 100);

	// Render score number under the label
	if (scoreNumberTexture) SDL_DestroyTexture(scoreNumberTexture);
	if (highScoreNumberTexture) SDL_DestroyTexture(highScoreNumberTexture);

	// Render number to surface (just to get width)
	std::string scoreStr = std::to_string(score);
	SDL_Surface* numSurface = TTF_RenderText_Solid(font, scoreStr.c_str(), color);
	scoreNumberTexture = SDL_CreateTextureFromSurface(renderer, numSurface);

	currentMode = mode; // Set the mode

	int displayedHighscore = (currentMode == GameMode::CAMPAIGN) ? campaignHighscore : endlessHighscore;
	std::string highScoreStr = std::to_string(displayedHighscore);
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

void ScoreManager::render(SDL_Renderer* renderer, GameMode mode)
{
	updateTexture(renderer, mode);
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
	if (shouldSpawnPickup)
	{
		shouldSpawnPickup = false;
		return true;
	}
	return false;
}

bool ScoreManager::giveLive()
{
	if (shouldGiveLive)
	{
		shouldGiveLive = false;
		SoundManager::playSound(SoundID::LIFE_UP);
		return true;
	}
	return false;
}

bool ScoreManager::isEndlessUnlocked() const
{
	return endlessUnlocked;
}

void ScoreManager::unlockEndless()
{
	if (!endlessUnlocked)
	{
		endlessUnlocked = true;
		saveHighScore((getExeDir() + "Assets\\highscore.txt").c_str());
	}
}

int ScoreManager::getStarCount() const
{
	int stars = 0;
	for (int i = 0; i < 5; i++)
	{
		if (endlessHighscore >= STAR_THRESHOLDS[i])
			stars++;
	}
	return stars;
}