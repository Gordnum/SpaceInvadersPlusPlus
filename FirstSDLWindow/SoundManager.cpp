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
    loadSound(SoundID::WEAPON_DEFAULT, "../Assets/SoundEffects/DEFAULT_SHOT.wav");
    loadSound(SoundID::WEAPON_BOMB, "../Assets/SoundEffects/bomb.wav");
    loadSound(SoundID::ENEMY_DEATH, "../Assets/SoundEffects/enemy_death.wav");
    loadSound(SoundID::BOMB_EXPLODE, "../Assets/SoundEffects/bomb_explode.wav");
    loadSound(SoundID::ENEMY_SHOOT, "../Assets/SoundEffects/enemy_shoot.wav");
    loadSound(SoundID::UFO_HOVER, "../Assets/SoundEffects/ufo_hover.wav");
    loadSound(SoundID::CHANGE_TO_DEFAULT, "../Assets/SoundEffects/change_to_default.wav");
    loadSound(SoundID::CHANGE_TO_PIERCING, "../Assets/SoundEffects/change_to_piercing.wav");
    loadSound(SoundID::CHANGE_TO_BOMB, "../Assets/SoundEffects/change_to_bomb.wav");
    loadSound(SoundID::CHANGE_TO_RAPID, "../Assets/SoundEffects/change_to_rapid.wav");
    loadSound(SoundID::CHANGE_TO_TRIPMINE, "../Assets/SoundEffects/change_to_tripmine.wav");
    loadSound(SoundID::TRIPMINE_SET_AND_EXPLODE, "../Assets/SoundEffects/tripmine_set_and_explode.wav");
    //will add more

    /*
    loadSound(SoundID::WEAPON_PIERCING, "../Assets/Sounds/piercing.wav");
    loadSound(SoundID::WEAPON_TRIPMINE, "../Assets/Sounds/tripmine.wav");
    loadSound(SoundID::WEAPON_RAPID, "../Assets/Sounds/rapid.wav");
    loadSound(SoundID::PLAYER_HIT, "../Assets/Sounds/player_hit.wav");
    loadSound(SoundID::BOSS_SHOOT, "../Assets/Sounds/boss_shoot.wav");
    loadSound(SoundID::PICKUP_COLLECT, "../Assets/Sounds/pickup.wav");
    */
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