#include "Boss.h"

const int SCREEN_WIDTH = 800;

Boss::Boss(SDL_Renderer* renderer)
	 : renderer(renderer), texture(nullptr), health(maxHealth), speed(100.0f), direction(1), active(false)
{
    SDL_Surface* surface = IMG_Load("../Assets/Textures/ufo.png");
    if (!surface)
    {
        SDL_Log("Failed to load boss texture: %s", IMG_GetError());
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);


    rect = { (SCREEN_WIDTH - rect.w) / 2, 100, 120, 60 };
}

Boss::~Boss()
{
    if (texture) SDL_DestroyTexture(texture);
}

void Boss::activate()
{
    active = true;
    health = maxHealth;
    rect.x = (SCREEN_WIDTH - rect.w) / 2;
    lastShotTime = SDL_GetTicks();
}

void Boss::deactivate() { active = false; }

bool Boss::isActive() const { return active; }

bool Boss::isDefeated() const { return health <= 0; }

void Boss::takeDamage(int amount)
{
    health -= amount;
    if (health <= 0)
    {
        health = 0;
    }
}

SDL_Rect Boss::getRect() const { return rect; }

bool Boss::shouldSpawnEnemies()
{
    unsigned int now = SDL_GetTicks();
    if (now - lastSummonTime >= summonCooldown)
    {
        lastSummonTime = now;
        return true;
    }
    return false;
}

void Boss::update(float deltaTime, SDL_Renderer* renderer, std::vector<std::unique_ptr<BossBullet>>& bullets)
{
    if (!active) return;

    rect.x += static_cast<int>(speed * direction * deltaTime);

    // movement
    if (rect.x <= 0)
    {
        rect.x = 0;
        direction = 1;
    }
    else if (rect.x + rect.w >= SCREEN_WIDTH)
    {
        rect.x = SCREEN_WIDTH - rect.w;
        direction = -1;
    }

    // shooting
    unsigned int now = SDL_GetTicks();
    if (now - lastShotTime >= shootCooldown)
    {
        lastShotTime = now;

        int originX = rect.x + rect.w / 2;
        int originY = rect.y + rect.h;

        int bulletAmount = 30;

        for (int i = 0; i < bulletAmount; ++i)
        {
            float angle = static_cast<float>(rand() % 180);
            float radians = angle * 3.14159265f / 180.0f;
            float speed = 200.0f;

            float vx = speed * cosf(radians);
            float vy = speed * sinf(radians);

            bullets.push_back(std::move(std::make_unique<BossBullet>(renderer, originX, originY, vx, vy)));
        }
    }
}

void Boss::render(SDL_Renderer* renderer)
{
    if (!active || !texture) return;

    // Draw boss
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    /*
    // Draw health bar
    SDL_Rect border = { rect.x, rect.y - 20, rect.w, 10 };
    SDL_Rect fill = { rect.x, rect.y - 20, rect.w * health / maxHealth, 10 };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red bar
    SDL_RenderFillRect(renderer, &fill);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white border
    SDL_RenderDrawRect(renderer, &border);
    */
}