#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <cstdlib>
#include "ScoreManager.h"

class FinalResults
{
	private:
        TTF_Font* font;

		bool active;
        bool countingFinished;
        bool newHighScoreAchieved;
        bool highScoreChecked;
        ScoreManager* scoreManager;

        int baseScore;
        int lifeBonus;
        int rapidBonus;
        int piercingBonus;
        int bombBonus;
        int tripmineBonus;
        int bossBonus;
        int totalScore;

        int displayBase;
        int displayLife;
        int displayRapidBonus;
        int displayPiercingBonus;
        int displayBombBonus;
        int displayTripmineBonus;
        int displayBoss;
        int displayTotal;

        unsigned int lastTick;

	public:
        FinalResults();
        void start(int score, int livesRemaining, int rapidAmmo, int piercingAmmo, int bombAmmo, int tripmineAmmo, ScoreManager* sm);
        void update();
        void render(SDL_Renderer* renderer);
        bool isActive();
        bool isFinished() const;
        int getFinalScore();
        void close();
};