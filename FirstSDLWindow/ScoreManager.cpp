#include "ScoreManager.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

ScoreManager::ScoreManager(SDL_Renderer* renderer, const std::string& fontPath, int fontSize)
	:score(0), font(nullptr), scoreLabelTexture(nullptr), color({ 255, 255, 255, 255 })
{
	font = TTF_OpenFont(fontPath.c_str(), fontSize);
	if (!font)
		SDL_Log("Failed to load font: %s", TTF_GetError());

	updateTexture(renderer);
}

ScoreManager::~ScoreManager()
{
	if (scoreLabelTexture) SDL_DestroyTexture(scoreLabelTexture);
	if (scoreNumberTexture) SDL_DestroyTexture(scoreNumberTexture);
	if (font) TTF_CloseFont(font);
}

void ScoreManager::addPoints(int amount) { score += amount; }

int ScoreManager::getScore() const { return score; }

SDL_Texture* ScoreManager::renderText(SDL_Renderer* renderer, const std::string& text, SDL_Rect& outRect, int y, bool centerX, int fixedX)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (!surface) return nullptr;

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);

	int x = centerX ? (SCREEN_WIDTH - surface->w) / 2 : fixedX;
	outRect = { x, y, surface->w, surface->h };

	SDL_FreeSurface(surface);
	return tex;
}

void ScoreManager::updateTexture(SDL_Renderer* renderer)
{
	// --- Render "Score" label ---
	if (scoreLabelTexture) SDL_DestroyTexture(scoreLabelTexture);
	scoreLabelTexture = renderText(renderer, "<SCORE>", scoreLabelRect, 20, false, 100); // left-aligned at x=100

	// --- Render score number under the label ---
	if (scoreNumberTexture) SDL_DestroyTexture(scoreNumberTexture);

	// Render number to surface (just to get width)
	std::string scoreStr = std::to_string(score);
	SDL_Surface* numSurface = TTF_RenderText_Solid(font, scoreStr.c_str(), color);
	scoreNumberTexture = SDL_CreateTextureFromSurface(renderer, numSurface);

	// Compute center x of label, then shift number to center under it
	int centerX = scoreLabelRect.x + scoreLabelRect.w / 2;
	int numX = centerX - numSurface->w / 2;
	int numY = scoreLabelRect.y + scoreLabelRect.h;

	scoreNumberRect = { numX, numY, numSurface->w, numSurface->h };
	SDL_FreeSurface(numSurface);

}

void ScoreManager::render(SDL_Renderer* renderer)
{
	updateTexture(renderer);
	if (scoreLabelTexture)
		SDL_RenderCopy(renderer, scoreLabelTexture, nullptr, &scoreLabelRect);
	if (scoreNumberTexture)
		SDL_RenderCopy(renderer, scoreNumberTexture, nullptr, &scoreNumberRect);
}