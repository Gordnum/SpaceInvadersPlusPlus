#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

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
		unsigned int waveIntroStartTime;
		unsigned waveIntroDuration;
		const unsigned bossWaveIntroDuration;
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


		float getEnemySpeed() const;
		float getProjectileSpeed() const;

		void renderBossIntro(SDL_Renderer* renderer);
};
