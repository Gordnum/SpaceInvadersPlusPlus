#pragma once
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class ComboManager
{
	private:
		float multiplier = 1.0f;
		const float MaxMultiplier = 5.0f;

		float comboBarProgress = 0.0f;
		float comboFillPerKill = 0.0f;
		float comboBarDecayRate = 0.0f;

		float shakeOffsetX = 0.0f;
		float shakeOffsetY = 0.0f;

		float scale = 1.0f;               // for pop effect
		bool scaleUp = false;
		unsigned int lastMultiplierUpdate = 0; // to time animations

		bool comboActive = false;
		unsigned int lastUpdateTime = 0;

		TTF_Font* font;
		SDL_Color color;

	public:
		ComboManager();
		~ComboManager();

		void update(float deltaTime);                    
		void render(SDL_Renderer* renderer); 
		void onEnemyKilled();            
		void reset();                    
		float getMultiplier() const;

		void updateDifficultyScaling();
};