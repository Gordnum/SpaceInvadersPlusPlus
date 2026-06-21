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

    bossDeath.push_back(IMG_LoadTexture(renderer, "../Assets/Textures/boss_death_1.png"));
    bossDeath.push_back(IMG_LoadTexture(renderer, "../Assets/Textures/boss_death_2.png"));


    rect = { 0, 100, 120, 60 };
    rect.x = (SCREEN_WIDTH - rect.w) / 2;
}

Boss::~Boss()
{
    if (texture) SDL_DestroyTexture(texture);

    for (auto tex : bossDeath)
    {
        if (tex) SDL_DestroyTexture(tex);
    }
    bossDeath.clear();
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

bool Boss::isDying() const { return dying; }

bool Boss::isDeathFinished() const
{
    return dying && SDL_GetTicks() - deathStartTime > 2000;
}

void Boss::startDeath()
{
    dying = true;
    SoundManager::playSound(SoundID::BOSS_DEATH);
    deathStartTime = SDL_GetTicks();
    lastExplosionSpawn = deathStartTime;
}

void Boss::updateDeath()
{
    unsigned int now = SDL_GetTicks();

    Uint32 elapsed = now - deathStartTime;
    bool isBlinkingPhase = elapsed < blinkDuration;

    if (isBlinkingPhase)
    {
        // frequency control here
        visible = ((elapsed / blinkInterval) % 2 == 0);

        //spawn boss death frames
        if (now - lastExplosionSpawn > 100)
        {
            lastExplosionSpawn = now;

            BossDeathExplosion e;
            e.rect.w = 32;
            e.rect.h = 32;
            e.rect.x = rect.x + rand() % rect.w;
            e.rect.y = rect.y + rand() % rect.h;
            e.startTime = now;
            e.lastFrameTime = now;

            explosions.push_back(e);
        }
    }
    else
    {
        visible = false;
    }

    //update boss death frames
    for (auto& e : explosions)
    {
        if (now - e.lastFrameTime > 100)
        {
            e.frame = (e.frame + 1) % bossDeath.size();
            e.lastFrameTime = now;
        }
    }

    //cleanup boss death frames
    explosions.erase(
        std::remove_if(explosions.begin(), explosions.end(),
            [now](const BossDeathExplosion& e)
            {
                return now - e.startTime > 300;
            }),
        explosions.end()
    );

}

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

    if (dying)
    {
        updateDeath();
        return;
    }

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

        SoundManager::playSound(SoundID::BOSS_SHOOT);
    }
}

void Boss::render(SDL_Renderer* renderer)
{
    if (!active || !texture) return;

    // Draw boss (with blinking)
    if (visible)
        SDL_RenderCopy(renderer, texture, nullptr, &rect);

    // Draw boss_death frames
    for (auto& e : explosions)
    {
        SDL_Texture* tex = bossDeath[e.frame];
        SDL_RenderCopy(renderer, tex, nullptr, &e.rect);
    }

    // Draw health bar (hide when boss is dying)
    if (!dying)
    {
        SDL_Rect border = { rect.x, rect.y - 20, rect.w, 10 };
        SDL_Rect fill = { rect.x, rect.y - 20, rect.w * health / maxHealth, 10 };

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red bar
        SDL_RenderFillRect(renderer, &fill);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white border
        SDL_RenderDrawRect(renderer, &border);
    }
}