#pragma once
#include <SDL.h>
#include <vector>

class Enemy
{
private:
SDL_Rect rect;
SDL_Renderer* renderer;
bool alive;


public:
Enemy(SDL_Renderer* renderer);
SDL_Rect getRect() const;
void update();
void render();
void destroy();
bool isAlive() const;
void setPosition(int x, int y);
void move(int dx, int dy);
};