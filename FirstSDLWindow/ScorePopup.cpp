#include "ScorePopup.h"

ScorePopup::ScorePopup(SDL_Renderer* renderer, const std::string& text, int x, int y, float multiplier, SDL_Color* overrideColor)
	: texture(nullptr), lifetime(0.0f)
{
	TTF_Font* font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 18);
	if (!font)
	{
		SDL_Log("Failed to load font: %s", TTF_GetError());
		return;
	}

	TTF_SetFontStyle(font, TTF_STYLE_BOLD);

	SDL_Color color;
	if (overrideColor)
		color = *overrideColor;
	else if (multiplier >= 5.0f)
		color = { 255, 0, 0 };		//red
	else if (multiplier >= 4.0f)
		color = { 255, 165, 0 };	//orange
	else if (multiplier >= 3.0f)
		color = { 255, 255, 0 };    // yellow
	else if (multiplier >= 2.0f)
		color = { 0, 255, 0 };		// green
	else
		color = { 255, 255, 255 };	// white

	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (!surface)
	{
		TTF_CloseFont(font);
		return;
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	rect = { x - surface->w / 2, y, surface->w, surface->h };

	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
}

ScorePopup::~ScorePopup()
{
	if (texture)
		SDL_DestroyTexture(texture);
}

void ScorePopup::update(float dt)
{
	lifetime += dt;

	rect.y -= static_cast<int>(speedY * dt);
}

void ScorePopup::render(SDL_Renderer* renderer)
{
	if (!texture)
		return;

	float alphaRatio = 1.0f - (lifetime / duration);

	if (alphaRatio < 0.0f) alphaRatio = 0.0f;

	SDL_SetTextureAlphaMod(texture, static_cast<unsigned short>(255 * alphaRatio));

	SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

bool ScorePopup::isAlive() const
{
	return lifetime < duration;
}