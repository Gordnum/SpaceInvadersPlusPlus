#pragma once
#include <SDL.h>

class WaveManager
{
	private:
		unsigned int currentWave;
		float enemySpeedMultiplier;
		float projectileSpeedMultiplier;
		bool showingWaveIntro;
		unsigned int waveIntroStartTime;
		const unsigned waveIntroDuration;

	public:
		WaveManager();

		void startWaveIntro();
		bool getWaveIntro() const;
		unsigned int getWaveIntroStartTime() const;
		const unsigned int getWaveIntroDuration() const;
		void setShowingWaveIntro();
		void nextWave();
		int getWave() const;

		float getEnemySpeed() const;
		float getProjectileSpeed() const;
};
