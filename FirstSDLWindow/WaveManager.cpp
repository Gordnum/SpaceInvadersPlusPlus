#include "WaveManager.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

WaveManager::WaveManager()
			:currentWave(1), enemySpeedMultiplier(10.0f), projectileSpeedMultiplier(300.0f), 
             showingWaveIntro(false), waveIntroStartTime(0), waveIntroDuration(2000)
{
    font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 48);
    if (!font)
        SDL_Log("Failed to load wave font: %s", TTF_GetError());
}

WaveManager::~WaveManager()
{
    if(font) TTF_CloseFont(font);
}

void WaveManager::startWaveIntro()
{
    showingWaveIntro = true;
    waveIntroStartTime = SDL_GetTicks();
}

void WaveManager::setShowingWaveIntro(bool state) { showingWaveIntro = state; }

void WaveManager::showWaveIntro(SDL_Renderer* renderer)
{
	SDL_Color color = { 255, 255, 255 };

	std::string introText = "WAVE " + std::to_string(getWave());
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, introText.c_str(), color);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	// Base size
	int baseW = textSurface->w;
	int baseH = textSurface->h;

	// Calculate pulse scale (between 0.9 and 1.2)
	float time = SDL_GetTicks() / 100.0f; // make it slower
	float scale = 1.0f + 0.1f * sin(time); // adjust amplitude

	int scaledW = static_cast<int>(baseW * scale);
	int scaledH = static_cast<int>(baseH * scale);

	SDL_Rect textRect =
	{
		SCREEN_WIDTH / 2 - scaledW / 2,
		SCREEN_HEIGHT / 2 - scaledH / 2,
		scaledW, scaledH
	};

	SDL_RenderCopyEx(renderer, textTexture, nullptr, &textRect, 0.0, nullptr, SDL_FLIP_NONE);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

void WaveManager::nextWave() 
{
    currentWave++;
    enemySpeedMultiplier += 2.0f;        // Increase enemy movement speed
    projectileSpeedMultiplier += 25.0f;  // Increase projectile speed
}

int WaveManager::getWave() const { return currentWave; }
float WaveManager::getEnemySpeed() const { return enemySpeedMultiplier; }
float WaveManager::getProjectileSpeed() const { return projectileSpeedMultiplier;  }

bool WaveManager::getWaveIntro() const { return showingWaveIntro; }
unsigned int WaveManager::getWaveIntroStartTime() const { return waveIntroStartTime; }
const unsigned int WaveManager::getWaveIntroDuration() const { return waveIntroDuration; }