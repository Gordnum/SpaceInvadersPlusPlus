#include <iostream>
#include "SoundManager.h"

std::map<SoundID, Mix_Chunk*> SoundManager::sounds;

void SoundManager::init() 
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
    {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        return;
    }
    loadAllSounds(); // automatically load all sounds

    Mix_AllocateChannels(64);
}

void SoundManager::loadSound(SoundID id, const std::string& filePath) 
{
    Mix_Chunk* sound = Mix_LoadWAV(filePath.c_str());
    if (!sound) 
    {
        SDL_Log("Failed to load sound %s: %s", filePath.c_str(), Mix_GetError());
        return;
    }
    sounds[id] = sound;
}

void SoundManager::playSound(SoundID id) 
{
    if (sounds.count(id)) 
    {
        Mix_PlayChannel(-1, sounds[id], 0);
    }
}

int SoundManager::playLoop(SoundID id)
{
    if (!sounds.count(id))
        return -1;

    return Mix_PlayChannel(-1, sounds[id], -1); // loop forever
}

void SoundManager::stopChannel(int channel, int fadeMs)
{
    if (channel != -1)
        Mix_HaltChannel(channel);
}

void SoundManager::loadAllSounds() 
{
    std::string base = getExeDir();

    loadSound(SoundID::WEAPON_DEFAULT, base + "Assets\\SoundEffects\\DEFAULT_SHOT.wav");
    loadSound(SoundID::WEAPON_PIERCING, base + "Assets\\SoundEffects\\piercing.wav");
    loadSound(SoundID::WEAPON_BOMB, base + "Assets\\SoundEffects\\bomb.wav");
    loadSound(SoundID::WEAPON_TRIPMINE, base + "Assets\\SoundEffects\\tripmine.wav");
    loadSound(SoundID::ENEMY_DEATH, base + "Assets\\SoundEffects\\enemy_death.wav");
    loadSound(SoundID::BOMB_EXPLODE, base + "Assets\\SoundEffects\\bomb_explode.wav");
    loadSound(SoundID::ENEMY_SHOOT, base + "Assets\\SoundEffects\\enemy_shoot.wav");
    loadSound(SoundID::UFO_HOVER, base + "Assets\\SoundEffects\\ufo_hover.wav");
    loadSound(SoundID::UFO_DEATH, base + "Assets\\SoundEffects\\ufo_death.wav");
    loadSound(SoundID::CHANGE_TO_DEFAULT, base + "Assets\\SoundEffects\\change_to_default.wav");
    loadSound(SoundID::CHANGE_TO_PIERCING, base + "Assets\\SoundEffects\\change_to_piercing.wav");
    loadSound(SoundID::CHANGE_TO_BOMB, base + "Assets\\SoundEffects\\change_to_bomb.wav");
    loadSound(SoundID::CHANGE_TO_RAPID, base + "Assets\\SoundEffects\\change_to_rapid.wav");
    loadSound(SoundID::CHANGE_TO_TRIPMINE, base + "Assets\\SoundEffects\\change_to_tripmine.wav");
    loadSound(SoundID::TRIPMINE_SET_AND_EXPLODE, base + "Assets\\SoundEffects\\tripmine_set_and_explode.wav");
    loadSound(SoundID::PLAYER_HIT, base + "Assets\\SoundEffects\\player_hit.wav");
    loadSound(SoundID::PICKUP_COLLECT, base + "Assets\\SoundEffects\\pickup_collect.wav");
    loadSound(SoundID::LIFE_UP, base + "Assets\\SoundEffects\\life_up.wav");
    loadSound(SoundID::BOSS_TAKE_DAMAGE, base + "Assets\\SoundEffects\\boss_take_damage.wav");
    loadSound(SoundID::BOSS_SHOOT, base + "Assets\\SoundEffects\\boss_shoot.wav");
    loadSound(SoundID::BOSS_WARNING, base + "Assets\\SoundEffects\\boss_warning.wav");
    loadSound(SoundID::BOSS_DEATH, base + "Assets\\SoundEffects\\boss_death.wav");
    loadSound(SoundID::BOSS_HOVER, base + "Assets\\SoundEffects\\boss_hover.wav");
    loadSound(SoundID::MENU_ENTER, base + "Assets\\SoundEffects\\menu_enter.wav");
    loadSound(SoundID::MENU_UPDOWN, base + "Assets\\SoundEffects\\menu_updown.wav");
    loadSound(SoundID::MENU_BACK, base + "Assets\\SoundEffects\\menu_back.wav");
    loadSound(SoundID::CAMPAIGN_INTRO, base + "Assets\\SoundEffects\\campaign_intro.wav");
}

void SoundManager::clean() 
{
    for (auto& pair : sounds) 
    {
        Mix_FreeChunk(pair.second);
    }
    sounds.clear();
    Mix_CloseAudio();
}