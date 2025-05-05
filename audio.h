#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_mixer.h>

extern Mix_Chunk* fallSound;
extern Mix_Chunk* landSound;
extern Mix_Chunk* shootSound;
extern Mix_Chunk* enemyHitSound;

void initAudio();
void playFallSound();
void playLandSound();
void playShootSound();
void playEnemyHitSound();
void freeAudio();

#endif
