#include "BulletManager.h"

BulletManager::BulletManager(SDL_Renderer* renderer)
			  :renderer(renderer)
{}

BulletManager::~BulletManager() { clear(); }

void BulletManager::fire(int x, int y, WeaponType currentWeapon, int ammo)
{
    Bullet* bullet = new Bullet(renderer);
    if (currentWeapon == WeaponType::PIERCING_SHOT)
        bullet->setWeapon(currentWeapon, ammo);  // assign ammo
    else
    {
        for (auto& bullet : bullets)
        {
            if (bullet->isActive() && bullet->getCurrentWeapon() == WeaponType::DEFAULT)
            {
                // makes it so only 1 bullet is active on the screen
                return;
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

    bullet->fire(x, y, currentWeapon);
    bullets.push_back(bullet);
}

void BulletManager::update()
{
    for (auto it = bullets.begin(); it != bullets.end();)
    {
        Bullet* bullet = *it;
        bullet->update();

        if (!bullet->isActive()) 
        {
            delete bullet;
            it = bullets.erase(it);
        }
        else 
        {
            ++it;
        }
    }
}

void BulletManager::render()
{
    for (Bullet* bullet : bullets)
    {
        bullet->render();
    }
}

void BulletManager::clear()
{
    for (Bullet* bullet : bullets)
    {
        delete bullet;
    }
    bullets.clear();
}

std::vector<Bullet*>& BulletManager::getBullets()
{
    return bullets;
}