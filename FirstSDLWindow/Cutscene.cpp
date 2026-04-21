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
        if (currentLine < static_cast<int>(lines.size()) - 1)
        {
            currentLine++;
            lastSwitchTime = now;
        }
        else
            finished = true;
    }
}

void Cutscene::render()
{
    if (!active) return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = { 255, 255, 255 };

    const int xCenter = 400;

    const int yPositions[] =
    {
        200, // first line
        300, // second line
        400  // third line
    };

    const unsigned int now = SDL_GetTicks();
    const unsigned int FADE_DURATION = 1000;

    for (int i = 0; i <= currentLine && i < static_cast<int>(lines.size()) && i < 3; i++)
    {
        unsigned int lineStartTime = lastSwitchTime;

        if (i < currentLine)
        {
            lineStartTime = lastSwitchTime - ((currentLine - i) * delayPerLine);
        }

        unsigned int elapsed = now - lineStartTime;

        unsigned short alpha = 255;

        if (elapsed < FADE_DURATION)
        {
            alpha = static_cast<unsigned short>((255.0f * elapsed) / FADE_DURATION);
        }

        SDL_Surface* surface = TTF_RenderText_Blended(font, lines[i].c_str(), { 255,255,255 });

        if (!surface)
            continue;

        int w = surface->w;
        int h = surface->h;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);

        if (!texture)
            continue;

        SDL_SetTextureAlphaMod(texture, alpha);

        SDL_Rect dest =
        {
            xCenter - w/2,
            yPositions[i] - h/2,
            w,
            h
        };

        SDL_RenderCopy(renderer, texture, nullptr, &dest);

        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

bool Cutscene::isActive() const { return active; }
bool Cutscene::isFinished() const { return finished; }