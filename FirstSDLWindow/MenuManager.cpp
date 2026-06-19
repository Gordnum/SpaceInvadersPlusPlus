#include "MenuManager.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

MenuManager::MenuManager(SDL_Renderer* renderer)
			:renderer(renderer), titleFont(nullptr), choicesFont(nullptr), currentState(MenuState::Main), selectedIndex(0), campaignBeaten(false), inMainMenu(true)
{
	titleFont = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 48);
	if (!titleFont)
	{
		SDL_Log("Failed to load font: %s", TTF_GetError());
	}

	choicesFont = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 24);
	if (!choicesFont)
	{
		SDL_Log("Failed to load font: %s", TTF_GetError());
	}
}

MenuManager::~MenuManager()
{
	if (titleFont) TTF_CloseFont(titleFont);
	if (choicesFont) TTF_CloseFont(choicesFont);
}

void MenuManager::setCampaignBeaten(bool beaten) { campaignBeaten = beaten; }
bool MenuManager::isCampaignBeaten() const { return campaignBeaten; }
void MenuManager::setInMainMenu(bool status) { inMainMenu = status; }
bool MenuManager::isInMainMenu() const { return inMainMenu; }

void MenuManager::handleEvent(const SDL_Event& e, bool& startCampaign, bool& startEndless, bool& quit)
{
	if (e.type == SDL_KEYDOWN)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_UP:
				selectedIndex--;
				if (selectedIndex < 0)
					selectedIndex = (currentState == MenuState::Main) ? 1 : (campaignBeaten ? 1 : 0);

				SoundManager::playSound(SoundID::MENU_UPDOWN);

				break;

			case SDLK_DOWN:
				selectedIndex++;
				if (currentState == MenuState::Main)
					selectedIndex = selectedIndex % 2;
				else
					selectedIndex = selectedIndex % (campaignBeaten ? 2 : 1);

				SoundManager::playSound(SoundID::MENU_UPDOWN);

				break;

			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				if (currentState == MenuState::Main)
				{
					if (selectedIndex == 0) 
					{
						currentState = MenuState::PlaySubMenu;
						selectedIndex = 0;
					}
					else if (selectedIndex == 1)
						quit = true;
				}
				else if (currentState == MenuState::PlaySubMenu)
				{
					if (selectedIndex == 0)
						startCampaign = true;
					else if (selectedIndex == 1 && campaignBeaten)
						startEndless = true;
				}

				SoundManager::playSound(SoundID::MENU_ENTER);

				break;

			case SDLK_ESCAPE:
				if (currentState == MenuState::PlaySubMenu) 
				{
					currentState = MenuState::Main;
					selectedIndex = 0;
				}
				break;
		}
	}
}

void MenuManager::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Title
	SDL_Color white = { 255, 255, 255 };
	SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont, "SPACE INVADERS++", white);
	SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
	SDL_Rect titleRect = { 400 - titleSurface->w / 2, 150, titleSurface->w, titleSurface->h };
	SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
	SDL_FreeSurface(titleSurface);
	SDL_DestroyTexture(titleTexture);

	if (currentState == MenuState::Main)
		renderMainMenu();
	else
		renderPlaySubMenu();

	SDL_RenderPresent(renderer);
}

void MenuManager::renderMainMenu()
{
	std::vector<std::string> options = { "Play", "Quit" };
	SDL_Color white = { 255, 255, 255 };
	SDL_Color yellow = { 255, 255, 0 };

	for (int i = 0; i < options.size(); ++i)
	{
		SDL_Color color = (i == selectedIndex) ? yellow : white;
		SDL_Surface* surface = TTF_RenderText_Solid(choicesFont, options[i].c_str(), color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_Rect rect = { 400 - surface->w / 2, 400 + i * 50, surface->w, surface->h };
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}

void MenuManager::renderPlaySubMenu()
{
	std::vector<std::string> options = { "Campaign" };
	if (campaignBeaten)
		options.push_back("Endless");

	SDL_Color white = { 255, 255, 255 };
	SDL_Color yellow = { 255, 255, 0 };

	for (int i = 0; i < options.size(); ++i)
	{
		SDL_Color color = (i == selectedIndex) ? yellow : white;
		SDL_Surface* surface = TTF_RenderText_Solid(choicesFont, options[i].c_str(), color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_Rect rect = { 400 - surface->w / 2, 300 + i * 50, surface->w, surface->h };
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}