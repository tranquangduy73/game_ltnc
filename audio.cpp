#include "audio.h"
#include <iostream>

Mix_Chunk* fallSound = NULL;
Mix_Chunk* landSound = NULL;
Mix_Chunk* shootSound = NULL;
Mix_Chunk* enemyHitSound = NULL;

void initAudio() {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    fallSound = Mix_LoadWAV("fall.wav");
    landSound = Mix_LoadWAV("land.wav");
    shootSound = Mix_LoadWAV("shoot.wav");
    enemyHitSound = Mix_LoadWAV("enemy_hit.wav");
    if (!fallSound || !landSound || !shootSound || !enemyHitSound) {
        std::cout << "Không thể load âm thanh! " << Mix_GetError() << std::endl;
    }
}

void playFallSound() {
    Mix_PlayChannel(-1, fallSound, 0);
}

void playLandSound() {
    Mix_PlayChannel(-1, landSound, 0);
}

void playShootSound() {
    Mix_PlayChannel(-1, shootSound, 0);
}

void playEnemyHitSound() {
    Mix_PlayChannel(-1, enemyHitSound, 0);
}

void freeAudio() {
    Mix_FreeChunk(fallSound);
    Mix_FreeChunk(landSound);
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(enemyHitSound);
    Mix_CloseAudio();
}
