#include "Player.h"
#include <string>

// note kalo dia function yang ngereturn sesuatu, valuenya gabisa diubah. Tapi kalo functionnya void, valuenya bisa diubah.

Player::Player(SDL_Renderer* renderer)
    :renderer(renderer), font(nullptr), speed(300.0f), movingLeft(false), movingRight(false),
    playerLives(3), currentTexture(weaponTextures[WeaponType::DEFAULT])
{
    rect = { 375, 475, 55, 28 };

    std::string base = getExeDir();

    font = TTF_OpenFont((base + "Assets\\Fonts\\space_invaders.ttf").c_str(), 20);
    if (!font)
        SDL_Log("Failed to load font: %s", TTF_GetError());

    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
}

Player::~Player()
{
    for (auto& pair : weaponTextures)
    {
        if (pair.second)
            SDL_DestroyTexture(pair.second);
    }
    weaponTextures.clear();

    for (auto& tex : deathFrames)
    {
        if (tex) SDL_DestroyTexture(tex);
    }
    deathFrames.clear();

    if (font)
        TTF_CloseFont(font);
}

void Player::handleEvent(const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
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

void Player::loadWeaponTextures(SDL_Renderer* renderer)
{
    std::string base = getExeDir();

    weaponTextures[WeaponType::DEFAULT] = IMG_LoadTexture(renderer, (base + "Assets\\Textures\\player_1.png").c_str());
    weaponTextures[WeaponType::PIERCING_SHOT] = IMG_LoadTexture(renderer, (base + "Assets\\Textures\\player_2.png").c_str());
    weaponTextures[WeaponType::BOMB_SHOT] = IMG_LoadTexture(renderer, (base + "Assets\\Textures\\player_2.png").c_str());
    weaponTextures[WeaponType::TRIPMINE] = IMG_LoadTexture(renderer, (base + "Assets\\Textures\\player_1.png").c_str());
    weaponTextures[WeaponType::RAPID_SHOT] = IMG_LoadTexture(renderer, (base + "Assets\\Textures\\player_1.png").c_str());

    // Fallback default
    currentTexture = weaponTextures[WeaponType::DEFAULT];
}

void Player::setWeaponTexture(WeaponType type)
{
    if (weaponTextures.count(type) && currentTexture != weaponTextures[type])
        currentTexture = weaponTextures[type];
}

void Player::loadDeathTextures(SDL_Renderer* renderer)
{
    std::string base = getExeDir();

    deathFrames.push_back(IMG_LoadTexture(renderer, (base + "Assets\\Textures\\player_death_1.png").c_str()));
    deathFrames.push_back(IMG_LoadTexture(renderer, (base + "Assets\\Textures\\player_death_2.png").c_str()));
}

void Player::startDeathAnimation()
{
    dying = true;
    deathFinished = false;
    deathStartTime = SDL_GetTicks();
    deathFrame = 0;
}

void Player::render()
{
    if (dying)
    {
        if (!deathFinished && deathFrame < deathFrames.size())
        {
            SDL_RenderCopy(renderer, deathFrames[deathFrame], nullptr, &rect);
        }
        return;
    }

    if (!visible) return;

    if (!font)
    {
        SDL_Log("Font not loaded; skipping text render");
        return;
    }

    if (visible)
    {
        if (currentTexture)
            SDL_RenderCopy(renderer, currentTexture, nullptr, &rect);
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    std::string labelText = "<LIVES>";
    SDL_Surface* labelSurface = TTF_RenderText_Solid(font, labelText.c_str(), { 255, 255, 255 });
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

void Player::update(float deltaTime)
{
    if (isInvincible && !dying)
    {
        unsigned int now = SDL_GetTicks();

        if (now - lastBlinkTime >= BLINK_INTERVAL)
        {
            visible = !visible;
            lastBlinkTime = now;
        }

        if (now - invincibleStartTime >= INVINCIBLE_DURATION)
        {
            isInvincible = false;
            visible = true;
        }
    }

    if (dying)
    {
        if (deathFrames.empty())
        {
            deathFinished = true;
            return;
        }

        unsigned int now = SDL_GetTicks();
        int elapsed = now - deathStartTime;

        deathFrame = elapsed / DEATH_FRAME_DURATION;

        if (deathFrame >= deathFrames.size())
        {
            deathFinished = true;
            return;
        }

        return;
    }

    if (movingLeft) rect.x -= static_cast<int>(speed * deltaTime);
    if (movingRight) rect.x += static_cast<int>(speed * deltaTime);

    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > 800) rect.x = 800 - rect.w;

}

void Player::loseLives()
{
    if (isInvincible || playerLives <= 0 || dying) return;

    SoundManager::playSound(SoundID::PLAYER_HIT);

    playerLives--;

    if (playerLives > 0)
    {
        isInvincible = true;
        invincibleStartTime = SDL_GetTicks();
        lastBlinkTime = invincibleStartTime;
    }
    else
    {
        dying = true;
        deathStartTime = SDL_GetTicks();
        deathFrame = 0;

        isInvincible = false;
        visible = true;
    }
}

void Player::reset() 
{
    playerLives = 3;
    movingLeft = false;
    movingRight = false;
    rect.x = 375;

    dying = false;
    deathFinished = false;

    isInvincible = false;
    visible = true;
}

void Player::plusLives() { if (playerLives > 0) playerLives++; }
int Player::getX() const { return rect.x + rect.w / 2; }
int Player::getY() const { return rect.y; }
int Player::getLives() const { return playerLives; }
bool Player::isDying() const { return dying; }
bool Player::isDeathAnimationFinished() const { return deathFinished; }
SDL_Rect Player::getRect() const { return rect; }