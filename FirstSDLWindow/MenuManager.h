#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "SoundManager.h"

enum class MenuState 
{
	Main,
	PlaySubMenu
};

class MenuManager 
{
	private:
		SDL_Renderer* renderer;
		TTF_Font* titleFont;
		TTF_Font* choicesFont;

		MenuState currentState;
		int selectedIndex;
		bool inMainMenu;

		bool campaignBeaten;

		void renderMainMenu();
		void renderPlaySubMenu();
	public:
		MenuManager(SDL_Renderer* renderer);
		~MenuManager();

		void handleEvent(const SDL_Event& e, bool& startCampaign, bool& startEndless, bool& quit);
		void render();
		void setCampaignBeaten(bool beaten);
		void setInMainMenu(bool status);
		bool isCampaignBeaten() const;
		bool isInMainMenu() const;
};