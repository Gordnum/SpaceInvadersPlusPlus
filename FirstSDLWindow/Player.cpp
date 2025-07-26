#include "Player.h"
#include <string>


// note kalo dia function yang ngereturn sesuatu, valuenya gabisa diubah. Tapi kalo functionnya void, valuenya bisa diubah.

Player::Player(SDL_Renderer* renderer)
       :renderer(renderer), font(nullptr), speed(5), movingLeft(false), movingRight(false), playerLives(3)
{
    font = TTF_OpenFont("../Assets/Fonts/space_invaders.ttf", 20);
    if (!font)
        SDL_Log("Failed to load font: %s", TTF_GetError());

    rect = { 400, 500, 50, 20 };
}

void Player::handleEvent(const SDL_Event& e)
{
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch(e.key.keysym.sym)
        {
            case SDLK_LEFT:
            case SDLK_a:
                movingLeft = true;
                break;
            case SDLK_RIGHT: 
            case SDLK_d:
                movingRight = true; 
                break;
        }
    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) 
    {
        switch (e.key.keysym.sym) 
        {
            case SDLK_LEFT:
            case SDLK_a:
                movingLeft = false;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                movingRight = false;
                break;
        }
    }
}

void Player::render()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderFillRect(renderer, &rect);

    std::string labelText = "<LIVES>";
    SDL_Surface* labelSurface = TTF_RenderText_Solid(font, labelText.c_str(), {255, 255, 255});
    SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);

    SDL_Rect labelRect = { 600, 20, labelSurface->w, labelSurface->h };  // top label
    SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);

    std::string livesText = std::to_string(getLives());
    SDL_Surface* livesSurface = TTF_RenderText_Solid(font, livesText.c_str(), { 255, 255, 255 });
    SDL_Texture* livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurface);

    // Align number centered below label
    SDL_Rect numberRect = 
    {
        labelRect.x + (labelRect.w - livesSurface->w) / 2, // center X
        labelRect.y + labelRect.h, // below the label
        livesSurface->w,
        livesSurface->h
    };

    SDL_RenderCopy(renderer, livesTexture, nullptr, &numberRect);

    SDL_FreeSurface(labelSurface);
    SDL_FreeSurface(livesSurface);
    SDL_DestroyTexture(labelTexture);
    SDL_DestroyTexture(livesTexture);

}

void Player::update()
{    
    if (movingLeft) rect.x -= speed;
    if (movingRight) rect.x += speed;
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > 800) rect.x = 800 - rect.w;
}

void Player::loseLives() { if(playerLives > 0) playerLives--; }
void Player::plusLives() { if (playerLives > 0) playerLives++; }
int Player::getX() const { return rect.x + rect.w / 2; }
int Player::getY() const { return rect.y; }
int Player::getLives() const { return playerLives; }
SDL_Rect Player::getRect() const { return rect; }

