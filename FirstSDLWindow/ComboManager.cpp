#include "ComboManager.h"
#include <stdio.h>

ComboManager::ComboManager()
			:font(nullptr){}

ComboManager::~ComboManager()
{

}

void ComboManager::updateDifficultyScaling()
{
	if(multiplier == 1.0f)
	{
		color = { 255, 255, 255 };
		comboFillPerKill = 0.7f;
		comboBarDecayRate = 0.05f;
	}
	else if(multiplier == 2.0f)
	{
		color = { 0, 255, 0 };
		comboFillPerKill = 0.5f;
		comboBarDecayRate = 0.1f;
	}
	else if(multiplier == 3.0f)
	{
		color = { 255, 255, 0 };
		comboFillPerKill = 0.4f;
		comboBarDecayRate = 0.15f;
	}
	else if(multiplier == 4.0f)
	{
		color = { 255, 165, 0 };
		comboFillPerKill = 0.3f;
		comboBarDecayRate = 0.2f;
	}
	else if(multiplier == 5.0f)
	{
		color = { 255, 0, 0 };
		comboFillPerKill = 0.2f;
		comboBarDecayRate = 0.25f;
	}
}

void ComboManager::onEnemyKilled()
{
	comboActive = true;
	updateDifficultyScaling();
	comboBarProgress += comboFillPerKill;
	float previousMultiplier = multiplier;

	lastMultiplierUpdate = SDL_GetTicks();
	scale = 1.5f;       // grow a bit
	scaleUp = true;

	if(comboBarProgress >= 1.0f)
	{
		if(multiplier < MaxMultiplier)
		{
			multiplier += 1.0f;

			if (multiplier > MaxMultiplier)
				multiplier = MaxMultiplier;
		}
			
		comboBarProgress -= 1.0f;
	}

	if(multiplier != previousMultiplier)
	{
		updateDifficultyScaling();
	}

	lastUpdateTime = SDL_GetTicks();
}

void ComboManager::update()
{
	if (!comboActive) return;

	unsigned int now = SDL_GetTicks();
	float deltaTime = (now - lastUpdateTime) / 1000.0f;
	lastUpdateTime = now;

	comboBarProgress -= deltaTime * comboBarDecayRate;

	if (comboBarProgress <= 0.0f) 
	{
		comboActive = false;
		multiplier = 1.0f;
		comboBarProgress = 0.0f;
		updateDifficultyScaling();
	}

	// Handling the shaky animation
	float intensity = std::min(multiplier, 5.0f); // limit shake strength

	// Random shaking within [-intensity, intensity]
	shakeOffsetX = ((rand() % 100) / 100.0f - 0.5f) * intensity * 2.0f;
	shakeOffsetY = ((rand() % 100) / 100.0f - 0.5f) * intensity * 2.0f;

	// Handle scale shrinking back to normal
	if (scaleUp)
	{
		unsigned int elapsed = SDL_GetTicks() - lastMultiplierUpdate;
		if (elapsed > 500)
		{
			scale = 1.0f;
			scaleUp = false;
		}
		else
		{
			scale = 1.5f - 0.5f * (elapsed / 500.0f); // interpolate down
		}
	}
}

void ComboManager::render(SDL_Renderer* renderer)
{
	if (!comboActive) return;

	int barWidth = 150;
	int barHeight = 15;
	int x = 600, y = 570;
	SDL_Rect fill = { x, y, static_cast<int>(barWidth * comboBarProgress), barHeight };
	SDL_Rect outline = { x, y, barWidth, barHeight };

	font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 20);
	if (!font)
		SDL_Log("Failed to load combo font: %s", TTF_GetError());

	std::string multText = "x" + std::to_string(multiplier).substr(0, 1);
	SDL_Surface* surface = TTF_RenderText_Solid(font, multText.c_str(), color);

	if (!surface) 
	{
		SDL_Log("Failed to render text surface: %s", TTF_GetError());
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	int centerX = outline.x + outline.w / 2;
	int X = centerX - surface->w / 2;
	int Y = outline.y + outline.h - 45;

	int texW = 0, texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

	float finalScale = scale;
	int scaledW = static_cast<int>(texW * finalScale);
	int scaledH = static_cast<int>(texH * finalScale);

	SDL_Rect destRect = { X + static_cast<int>(shakeOffsetX), Y + static_cast<int>(shakeOffsetY), scaledW, scaledH };

	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, nullptr, &destRect);
	SDL_DestroyTexture(texture);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &fill);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &outline);

}

void ComboManager::reset()
{
	multiplier = 1.0f;
	comboBarProgress = 0.0f;
	comboActive = false;
}

float ComboManager::getMultiplier() const
{
	printf("Current multiplier: %.2f", multiplier);
	return multiplier;
}