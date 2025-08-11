#pragma once
#include <SDL.h>

class BossBullet 
{
    private:
        SDL_Texture* texture;
        SDL_Rect rect;
        float x, y;
        float vx, vy;
        bool active;

    public:
        BossBullet(SDL_Renderer* renderer, int startX, int startY, float velX, float velY);
        ~BossBullet();

        void update(float deltaTime);
        void render(SDL_Renderer* renderer) const;
        bool isActive() const;
        void deactivate();
        SDL_Rect getRect() const;
};