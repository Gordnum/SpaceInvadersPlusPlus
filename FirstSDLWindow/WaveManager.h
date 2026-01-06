#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class WaveManager
{
	private:
		unsigned int currentWave;
		float enemySpeedMultiplier;
		float projectileSpeedMultiplier;
		bool showingWaveIntro;
		unsigned int waveIntroStartTime;
		const unsigned waveIntroDuration;
		TTF_Font* font;

	public:
		WaveManager();
		~WaveManager();

		void startWaveIntro();
		bool getWaveIntro() const;
		unsigned int getWaveIntroStartTime() const;
		const unsigned int getWaveIntroDuration() const;
		void setShowingWaveIntro(bool state);
		void showWaveIntro(SDL_Renderer* renderer);
		void nextWave();
		int getWave() const;

		float getEnemySpeed() const;
		float getProjectileSpeed() const;
};
