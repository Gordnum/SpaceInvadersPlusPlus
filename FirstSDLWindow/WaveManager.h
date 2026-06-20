#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "SoundManager.h"

enum class waveIntroType
{
	NORMAL,
	BOSS
};

class WaveManager
{
	private:
		unsigned int currentWave;
		float enemySpeedMultiplier;
		float projectileSpeedMultiplier;
		bool showingWaveIntro;
		bool bossWarningPlayed;
		unsigned int waveIntroStartTime;
		unsigned int waveIntroDuration;
		const unsigned int bossWaveIntroDuration;
		TTF_Font* font;
		waveIntroType introType;

	public:
		WaveManager();
		~WaveManager();

		void startWaveIntro(waveIntroType type = waveIntroType::NORMAL);
		waveIntroType getIntroType() const;
		bool getWaveIntro() const;
		unsigned int getWaveIntroStartTime() const;
		const unsigned int getWaveIntroDuration() const;
		void setShowingWaveIntro(bool state);
		void showWaveIntro(SDL_Renderer* renderer);
		void nextWave();
		int getWave() const;
		void reset();

		float getEnemySpeed() const;
		float getProjectileSpeed() const;

		void renderBossIntro(SDL_Renderer* renderer);
};
