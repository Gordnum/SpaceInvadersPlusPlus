#include "FinalResults.h"
#include <SDL_ttf.h>
#include <string>

FinalResults::FinalResults()
{
    font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 20);

    if (!font)
        SDL_Log("Failed to load ammo font: %s", TTF_GetError());

    active = false;
    countingFinished = false;
    newHighScoreAchieved = false;
    scoreManager = nullptr;

    baseScore = 0;
    lifeBonus = 0;
    rapidBonus = 0;
    piercingBonus = 0;
    bombBonus = 0;
    tripmineBonus = 0;
    bossBonus = 0;
    totalScore = 0;

    displayBase = 0;
    displayLife = 0;
    displayRapidBonus = 0;
    displayPiercingBonus = 0;
    displayBombBonus = 0;
    displayTripmineBonus = 0;
    displayBoss = 0;
    displayTotal = 0;

    lastTick = SDL_GetTicks();
}

void FinalResults::start(int score, int livesRemaining, int rapidAmmo, int piercingAmmo, int bombAmmo, int tripmineAmmo, ScoreManager* sm)
{
    active = true;
    countingFinished = false;

    baseScore = score;

    scoreManager = sm;

    highScoreChecked = false;
    newHighScoreAchieved = false;

    // bonus rules
    lifeBonus = livesRemaining * 1000;

    rapidBonus = rapidAmmo * 100;

    piercingBonus = piercingAmmo * 200;

    bombBonus = bombAmmo * 300;

    tripmineBonus = tripmineAmmo * 400;

    bossBonus = 5000;

    totalScore = baseScore + lifeBonus + rapidBonus + piercingBonus + bombBonus + tripmineBonus + bossBonus;

    displayBase = 0;
    displayLife = 0;
    displayRapidBonus = 0;
    displayPiercingBonus = 0;
    displayBombBonus = 0;
    displayTripmineBonus = 0;
    displayBoss = 0;
    displayTotal = 0;

    lastTick = SDL_GetTicks();
}

void FinalResults::update()
{
    if (!active) 
        return;

    unsigned int now = SDL_GetTicks();

    float deltaTime = (now - lastTick) / 1000.0f;

    if (now - lastTick < 20) 
        return;

    lastTick = now;

    const float SPEED = 20000.0f;

    if (displayBase < baseScore)
    {
        displayBase += static_cast<int>(SPEED * deltaTime);
        if (displayBase > baseScore)
            displayBase = baseScore;
    }
    else if (displayLife < lifeBonus)
    {
        displayLife += static_cast<int>((SPEED - 4000.0f) * deltaTime);
        if (displayLife > lifeBonus)
            displayLife = lifeBonus;
    }
    else if (displayRapidBonus < rapidBonus)
    {
        displayRapidBonus += static_cast<int>((SPEED - 2500.0f) * deltaTime);

        if (displayRapidBonus > rapidBonus)
            displayRapidBonus = rapidBonus;
    }
    else if (displayPiercingBonus < piercingBonus)
    {
        displayPiercingBonus += static_cast<int>((SPEED - 2500.0f) * deltaTime);

        if (displayPiercingBonus > piercingBonus)
            displayPiercingBonus = piercingBonus;
    }
    else if (displayBombBonus < bombBonus)
    {
        displayBombBonus += static_cast<int>((SPEED - 2500.0f) * deltaTime);

        if (displayBombBonus > bombBonus)
            displayBombBonus = bombBonus;
    }
    else if (displayTripmineBonus < tripmineBonus)
    {
        displayTripmineBonus += static_cast<int>((SPEED - 2500.0f) * deltaTime);

        if (displayTripmineBonus > tripmineBonus)
            displayTripmineBonus = tripmineBonus;
    }
    else if (displayBoss < bossBonus)
    {
        displayBoss += static_cast<int>((SPEED - 2500.0f) * deltaTime);
        if (displayBoss > bossBonus)
            displayBoss = bossBonus;
    }
    else if (displayTotal < totalScore)
    {
        displayTotal += static_cast<int>(SPEED * deltaTime);
        if (displayTotal > totalScore)
            displayTotal = totalScore;
    }

    if (
            displayBase >= baseScore &&
            displayLife >= lifeBonus &&
            displayRapidBonus >= rapidBonus &&
            displayPiercingBonus >= piercingBonus &&
            displayBombBonus >= bombBonus &&
            displayTripmineBonus >= tripmineBonus &&
            displayBoss >= bossBonus &&
            displayTotal >= totalScore
        )
    {
        countingFinished = true;
    }

    
    if (countingFinished && !highScoreChecked)
    {
        highScoreChecked = true;

        if (totalScore > scoreManager->getHighScore())
        {
            scoreManager->setHighScore(totalScore);
            newHighScoreAchieved = true;
            SDL_Log("NEW HIGH SCORE: %d", totalScore);
        }
    }
}

bool FinalResults::isActive()
{
    return active;
}

int FinalResults::getFinalScore()
{
    return totalScore;
}

bool FinalResults::isFinished() const
{
    return countingFinished;
}

void FinalResults::render(SDL_Renderer* renderer)
{
    const int labelColumn = 400;
    const int separatorColumn = 420;
    const int valueColumn = 450;
    
    auto drawText = [](SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y)
    {
        SDL_Color white = { 255,255,255,255 };

        SDL_Surface* surface =
            TTF_RenderText_Solid
            (
                font,
                text.c_str(),
                white
            );

        SDL_Texture* texture =
            SDL_CreateTextureFromSurface
            (
                renderer,
                surface
            );

        SDL_Rect rect =
        {
            x,
            y,
            surface->w,
            surface->h
        };

        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    };

    auto drawRightAligned = [&](const std::string& text, int rightX, int y)
    {
        int w, h;
        TTF_SizeText(font, text.c_str(), &w, &h);

        drawText(renderer, font, text, rightX - w, y);
    };

    auto drawCentered = [&](const std::string& text, int y)
    {
        int w, h;
        TTF_SizeText(font, text.c_str(), &w, &h);

        drawText
        (
            renderer,
            font,
            text,
            (800 - w) / 2,
            y
        );
    };

    if (!active) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);

    SDL_Rect background =
    {
        150,
        90,
        500,
        460
    };

    SDL_RenderFillRect(renderer, &background);

    drawCentered("MISSION COMPLETE", 100);

    drawRightAligned("Base Score", labelColumn, 150);
    drawText(renderer, font, "|", separatorColumn, 150);
    drawText(renderer, font, std::to_string(displayBase), valueColumn, 150);

    drawRightAligned("Life Bonus", labelColumn, 190);
    drawText(renderer, font, "|", separatorColumn, 190);
    drawText(renderer, font, std::to_string(displayLife), valueColumn, 190);

    drawRightAligned("Rapid Ammo Bonus", labelColumn, 230);
    drawText(renderer, font, "|", separatorColumn, 230);
    drawText(renderer, font, std::to_string(displayRapidBonus), valueColumn, 230);

    drawRightAligned("Piercing Bonus", labelColumn, 270);
    drawText(renderer, font, "|", separatorColumn, 270);
    drawText(renderer, font, std::to_string(displayPiercingBonus), valueColumn, 270);

    drawRightAligned("Bomb Bonus", labelColumn, 310);
    drawText(renderer, font, "|", separatorColumn, 310);
    drawText(renderer, font, std::to_string(displayBombBonus), valueColumn, 310);

    drawRightAligned("Tripmine Bonus", labelColumn, 350);
    drawText(renderer, font, "|", separatorColumn, 350);
    drawText(renderer, font, std::to_string(displayTripmineBonus), valueColumn, 350);

    drawRightAligned("Boss Bonus", labelColumn, 390);
    drawText(renderer, font, "|", separatorColumn, 390);
    drawText(renderer, font, std::to_string(displayBoss), valueColumn, 390);

    drawRightAligned("Total Score", labelColumn, 450);
    drawText(renderer, font, "|", separatorColumn, 450);
    drawText(renderer, font, std::to_string(displayTotal), valueColumn, 450);

    if (newHighScoreAchieved)
    {
        unsigned int ticks = SDL_GetTicks();

        if ((ticks / 300) % 2 == 0)
        {
            drawCentered("*NEW HIGHSCORE ACHIEVED*", 40);
        }
    }

    if (countingFinished)
        drawCentered("PRESS ENTER TO CONTINUE", 540);
}

void FinalResults::close()
{
    active = false;
    countingFinished = false;
}