#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <map>
#include "WeaponType.h"
#include "SoundManager.h"

class Player
{
	private:
		SDL_Rect rect;
		SDL_Renderer* renderer;
		TTF_Font* font;
		float speed;
		bool movingLeft;
		bool movingRight;
		int playerLives;
		std::map<WeaponType, SDL_Texture*> weaponTextures;
		SDL_Texture* currentTexture;
		bool isInvincible = false;
		unsigned int invincibleStartTime = 0;
		unsigned int lastBlinkTime = 0;
		bool visible = true;

		static constexpr unsigned int INVINCIBLE_DURATION = 1200;
		static constexpr unsigned int BLINK_INTERVAL = 100;

	public:
		Player(SDL_Renderer* renderer);
		~Player();
		void handleEvent(const SDL_Event& e);
		void render();
		void update(float deltaTime);
		void loseLives();
		void plusLives();
		void loadWeaponTextures(SDL_Renderer* renderer);
		void setWeaponTexture(WeaponType type);
		int getX() const;
		int getY() const;
		int getLives() const;
		SDL_Rect getRect() const;
};