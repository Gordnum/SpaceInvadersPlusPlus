#include "Cutscene.h"

Cutscene::Cutscene(SDL_Renderer* r) : renderer(r), currentLine(0), active(false), 
                                      finished(false), lastSwitchTime(0), delayPerLine(2500)
{
	font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 24);
    if (!font)
    {
        SDL_Log("Font failed: %s", TTF_GetError());
    }
}

void Cutscene::start(const std::vector<std::string>& newLines)
{
    lines = newLines;
    currentLine = 0;
    active = true;
    finished = false;
    lastSwitchTime = SDL_GetTicks();

}

void Cutscene::update()
{
    if (!active || finished) return;

    unsigned int now = SDL_GetTicks();

    if (now - lastSwitchTime >= delayPerLine)
    {
        currentLine++;
        lastSwitchTime = now;

        if (currentLine >= (int)lines.size())
        {
            finished = true;
            active = false;
        }
    }
}

void Cutscene::render()
{
    if (!active) return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = { 255, 255, 255 };

    if (currentLine < (int)lines.size())
    {
        SDL_Surface* surface = TTF_RenderText_Solid(font, lines[currentLine].c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dest = {
            400 - surface->w / 2,
            300 - surface->h / 2,
            surface->w,
            surface->h
        };

        SDL_FreeSurface(surface);
        SDL_RenderCopy(renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

bool Cutscene::isActive() const { return active; }
bool Cutscene::isFinished() const { return finished; }