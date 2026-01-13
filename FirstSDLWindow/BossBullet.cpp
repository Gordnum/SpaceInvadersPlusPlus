#include "BossBullet.h"

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 800;
constexpr int PLAYFIELD_UPPER_MARGIN = 75;
constexpr int PLAYFIELD_BOTTOM_MARGIN = 90;
constexpr int BORDER_THICKNESS = 4;
constexpr int PLAYFIELD_TOP_Y = PLAYFIELD_UPPER_MARGIN + BORDER_THICKNESS;
constexpr int PLAYFIELD_BOTTOM_Y = SCREEN_HEIGHT - PLAYFIELD_BOTTOM_MARGIN;

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
    if (rect.y < PLAYFIELD_TOP_Y - rect.h || rect.y > PLAYFIELD_BOTTOM_Y || rect.x < -rect.w || rect.x > SCREEN_WIDTH)
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