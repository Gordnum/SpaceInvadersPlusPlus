#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <map>
#include "WeaponType.h"

class Player
{
	private:
		SDL_Rect rect;
		SDL_Renderer* renderer;
		TTF_Font* font;
		int speed;
		bool movingLeft;
		bool movingRight;
		int playerLives;
		std::map<WeaponType, SDL_Texture*> weaponTextures;
		SDL_Texture* currentTexture;

	public:
		Player(SDL_Renderer* renderer);
		~Player();
		void handleEvent(const SDL_Event& e);
		void render();
		void update();
		void loseLives();
		void plusLives();
		void loadWeaponTextures(SDL_Renderer* renderer);
		void setWeaponTexture(WeaponType type);
		int getX() const;
		int getY() const;
		int getLives() const;
		SDL_Rect getRect() const;
};