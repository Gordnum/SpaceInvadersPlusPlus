#include "WaveManager.h"

WaveManager::WaveManager()
			:currentWave(1), enemySpeedMultiplier(10.0f), projectileSpeedMultiplier(5.0f), 
             showingWaveIntro(false), waveIntroStartTime(0), waveIntroDuration(2000)
{

}

void WaveManager::startWaveIntro()
{
    showingWaveIntro = true;
    waveIntroStartTime = SDL_GetTicks();
}

void WaveManager::setShowingWaveIntro() { showingWaveIntro = false; }

void WaveManager::nextWave() 
{
    currentWave++;
    enemySpeedMultiplier += 5.0f;        // Increase enemy movement speed
    projectileSpeedMultiplier += 0.3f;  // Increase projectile speed (can be more aggressive)
}

int WaveManager::getWave() const { return currentWave; }
float WaveManager::getEnemySpeed() const { return enemySpeedMultiplier; }
float WaveManager::getProjectileSpeed() const { return projectileSpeedMultiplier;  }

bool WaveManager::getWaveIntro() const { return showingWaveIntro; }
unsigned int WaveManager::getWaveIntroStartTime() const { return waveIntroStartTime; }
const unsigned int WaveManager::getWaveIntroDuration() const { return waveIntroDuration; }