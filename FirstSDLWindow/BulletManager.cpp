#include "BulletManager.h"
#include "WeaponInventory.h"

BulletManager::BulletManager(SDL_Renderer* renderer)
			  :renderer(renderer)
{
    // Object Pooling 
    const int MAX_BULLETS = 200;
    for (int i = 0; i < MAX_BULLETS; ++i) 
    {
        bullets.push_back(std::make_unique<Bullet>(renderer));
    }

}

BulletManager::~BulletManager() { clear(); }

void BulletManager::fire(int x, int y, WeaponType currentWeapon, int ammo)
{
    auto bullet = std::make_unique<Bullet>(renderer);
    WeaponInventory inventory;

    if (currentWeapon != WeaponType::DEFAULT)
        inventory.addWeapon(currentWeapon, ammo);  // assign ammo
    else
    {
        for (const auto& bullet : bullets)
        {
            if (bullet->isActive() && bullet->getCurrentWeapon() == WeaponType::DEFAULT)
            {
                return; // makes it so only 1 bullet is active on the screen
            }
        }
    }

    for (auto& bullet : bullets)
    {
        if (!bullet->isActive())
        {
            bullet->fire(x, y, currentWeapon);
            break;
        }
    }

    bullets.push_back(std::make_unique<Bullet>(renderer));
    bullet->fire(x, y, currentWeapon);
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