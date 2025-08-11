#include "BossBullet.h"

BossBullet::BossBullet(SDL_Renderer* renderer, int startX, int startY, float velX, float velY)
		   :x(static_cast<float>(startX)), y(static_cast<float>(startY)), vx(velX), vy(velY), active(true), texture(nullptr)
{
	rect = { startX, startY, 5, 10 };
}

BossBullet::~BossBullet()
{

}

void BossBullet::update(float deltaTime)
{
    if (!active) return;

    x += vx * deltaTime;
    y += vy * deltaTime;

    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);

    // deactivate if off screen
    if (x < 0 || x > 800 || y < 0 || y > 600)
        deactivate();
}

void BossBullet::render(SDL_Renderer* renderer) const
{
    if (active) 
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // red color
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool BossBullet::isActive() const { return active; }
void BossBullet::deactivate() { active = false; }
SDL_Rect BossBullet::getRect() const { return rect; }