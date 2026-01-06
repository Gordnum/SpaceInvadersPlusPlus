#include "BulletManager.h"
#include "WeaponInventory.h"

BulletManager::BulletManager(SDL_Renderer* renderer)
			  :renderer(renderer)
{
    // Object Pooling 
    const int MAX_BULLETS = 200;
    bullets.reserve(MAX_BULLETS);
    for (int i = 0; i < MAX_BULLETS; ++i) 
    {
        bullets.push_back(std::make_unique<Bullet>(renderer));
    }

}

BulletManager::~BulletManager() { clear(); }

bool BulletManager::fire(int x, int y, WeaponType currentWeapon, int ammo)
{
    auto bullet = std::make_unique<Bullet>(renderer);
    WeaponInventory inventory;

    if (currentWeapon == WeaponType::DEFAULT)
    {
        for (const auto& b : bullets)
        {
            if (b->isActive() && b->getCurrentWeapon() == WeaponType::DEFAULT)
                return false;
        }
    }

    for (auto& b : bullets)
    {
        if (!b->isActive())
        {
            b->fire(x, y, currentWeapon);
            return true; // fired successfully
        }
    }

    bullets.push_back(std::make_unique<Bullet>(renderer));
    bullet->fire(x, y, currentWeapon);

    return false;
}

void BulletManager::update(float deltaTime)
{
    for (auto& bullet : bullets)
    {
        if (bullet->isActive())
            bullet->update(deltaTime);
    }
}

void BulletManager::render()
{
    for (const auto& bullet : bullets)
    {
        if (bullet->isActive())
            bullet->render();
    }
}

void BulletManager::clear()
{
    bullets.clear();
}

std::vector<std::unique_ptr<Bullet>>& BulletManager::getBullets() { return bullets; }