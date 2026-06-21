#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "SoundManager.h"
#include "ScoreManager.h"

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
		TTF_Font* creditsFont;

		MenuState currentState;
		int selectedIndex;
		bool inMainMenu;

		bool campaignBeaten;

		void renderMainMenu();
		void renderPlaySubMenu();

		ScoreManager* scoreManager;

	public:
		MenuManager(SDL_Renderer* renderer, ScoreManager* scoreManager);
		~MenuManager();

		void handleEvent(const SDL_Event& e, bool& startCampaign, bool& startEndless, bool& quit);
		void render();
		void setInMainMenu(bool status);
		bool isInMainMenu() const;
};