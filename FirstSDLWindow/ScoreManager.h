#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "Player.h"
#include "SoundManager.h"
#include "GameMode.h"

class ScoreManager
{
private:
	int score;
	int campaignHighscore;
	int endlessHighscore;
	int nextPickupThreshold;
	int nextGiveLivesThreshold;
	bool shouldSpawnPickup;
	bool shouldGiveLive;
	bool endlessUnlocked; //for flagging completed campaign or not
	GameMode currentMode;
	TTF_Font* font;
	SDL_Color color;
	SDL_Rect scoreLabelRect;
	SDL_Rect scoreNumberRect;
	SDL_Rect highScoreLabelRect;
	SDL_Rect highScoreNumberRect;
	SDL_Texture* scoreLabelTexture;
	SDL_Texture* scoreNumberTexture;
	SDL_Texture* highScoreLabelTexture;
	SDL_Texture* highScoreNumberTexture;

	SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, SDL_Rect& outRect, int y, bool centerX = false, int fixedX = 0);

	void updateTexture(SDL_Renderer* renderer, GameMode mode);

public:
	ScoreManager(SDL_Renderer* renderer);
	~ScoreManager();

	void addPoints(int amount);
	void render(SDL_Renderer* renderer, GameMode mode);
	int getScore() const;
	bool spawnPickup();
	bool giveLive();

	void awardScore(int points);

	void saveHighScore(const std::string& filename);
	void loadHighScore(const std::string& filename);
	void setCampaignHighScore(int value);
	void setEndlessHighScore(int value);
	int getCampaignHighScore() const;
	int getEndlessHighScore() const;

	bool isEndlessUnlocked() const;
	void unlockEndless();

	void reset();
};