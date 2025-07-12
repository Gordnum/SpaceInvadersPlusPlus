#include "Player.h"
#include <iostream>

// note kalo dia function yang ngereturn sesuatu, valuenya gabisa diubah. Tapi kalo functionnya void, valuenya bisa diubah.

Player::Player(SDL_Renderer* renderer)
       :renderer(renderer), speed(5), movingLeft(false), movingRight(false), playerLives(1)
{
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
}

void Player::update()
{    
    if (movingLeft) rect.x -= speed;
    if (movingRight) rect.x += speed;
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > 800) rect.x = 800 - rect.w;
}
void Player::loseLives()
{
    if(playerLives > 0) playerLives--;
}

int Player::getX() const
{
    return rect.x + rect.w / 2;
}

int Player::getY() const
{
    return rect.y;
}

int Player::getLives() const
{
    return playerLives;
}

SDL_Rect Player::getRect() const
{
    return rect;
}