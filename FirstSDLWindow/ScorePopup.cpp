#include "ScorePopup.h"

ScorePopup::ScorePopup(SDL_Renderer* renderer, const std::string& text, int x, int y)
		   : texture(nullptr), lifetime(0.0f)
{
	TTF_Font* font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 18);
	SDL_Color color = { 255, 255, 255, 255 }; //white

	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);

	rect = {x - surface->w / 2, y, surface->w, surface->h};

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
	float alphaRatio = 1.0f - (lifetime / duration);
	if (alphaRatio < 0.0f) alphaRatio = 0.0f;

	SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(255 * alphaRatio));

	SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

bool ScorePopup::isAlive() const
{
	return lifetime < duration;
}