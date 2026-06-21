#pragma once
#include <string>
#include <map>
#include <SDL_mixer.h>
#include <SDL.h>
#include "WeaponType.h"

enum class SoundID 
{
    WEAPON_DEFAULT,
    WEAPON_PIERCING,
    WEAPON_BOMB,
    WEAPON_TRIPMINE,
    WEAPON_RAPID,
    CHANGE_TO_DEFAULT,
    CHANGE_TO_PIERCING,
    CHANGE_TO_BOMB,
    CHANGE_TO_TRIPMINE,
    CHANGE_TO_RAPID,
    TRIPMINE_SET_AND_EXPLODE,
    ENEMY_SHOOT,
    ENEMY_DEATH,
    PLAYER_HIT,
    BOSS_SHOOT,
    BOSS_TAKE_DAMAGE,
    PICKUP_COLLECT,
    LIFE_UP,
    BOMB_EXPLODE,
    UFO_HOVER,
    UFO_DEATH,
    MENU_ENTER,
    MENU_UPDOWN,
    MENU_BACK,
    CAMPAIGN_INTRO,
    BOSS_WARNING,
    BOSS_DEATH
};

inline SoundID weaponTypeToSoundID(WeaponType weapon) 
{
    switch (weapon) 
    {
        case WeaponType::DEFAULT:        return SoundID::WEAPON_DEFAULT;
        case WeaponType::PIERCING_SHOT:  return SoundID::WEAPON_PIERCING;
        case WeaponType::BOMB_SHOT:      return SoundID::WEAPON_BOMB;
        case WeaponType::TRIPMINE:       return SoundID::WEAPON_TRIPMINE;
        case WeaponType::RAPID_SHOT:     return SoundID::WEAPON_DEFAULT;
        default:                         return SoundID::WEAPON_DEFAULT;
    }
}

inline SoundID weaponTypeToSwitchSound(WeaponType type)
{
    switch (type)
    {
        case WeaponType::DEFAULT:        return SoundID::CHANGE_TO_DEFAULT;
        case WeaponType::PIERCING_SHOT:  return SoundID::CHANGE_TO_PIERCING;
        case WeaponType::BOMB_SHOT:      return SoundID::CHANGE_TO_BOMB;
        case WeaponType::TRIPMINE:       return SoundID::CHANGE_TO_TRIPMINE;
        case WeaponType::RAPID_SHOT:     return SoundID::CHANGE_TO_RAPID;
        default:                         return SoundID::CHANGE_TO_DEFAULT;
    }
}

class SoundManager 
{
    public:
        static void init();
        static void loadSound(SoundID id, const std::string& filePath);
        static void playSound(SoundID id);
        static void clean();
        static int playLoop(SoundID id);
        static void stopChannel(int channel, int fadeMs = 250);

    private:
        static void loadAllSounds();
        static std::map<SoundID, Mix_Chunk*> sounds;
};
