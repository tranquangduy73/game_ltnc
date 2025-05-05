#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 800;
const int PLAYER_WIDTH = 48;
const int PLAYER_HEIGHT = 48;
const float GRAVITY = 0.5f;
const int MOVE_SPEED = 10;
const int JUMP_VELOCITY = -15;
const int BULLET_COOLDOWN = 500;
const int BULLET_SPEED = 10;
const int BULLET_SCALE = 2;
const int MOVING_PLATFORM_SPEED = 3;
const int ENEMY_JUMP_SPEED = 2;
const int ENEMY_JUMP_HEIGHT = 20;

extern int score;
extern float velY;
extern int highestY;
extern TTF_Font* font;
extern SDL_Color textColor;
extern SDL_Rect player;

extern SDL_Rect playAgainRect;
extern SDL_Rect menuRect;
extern bool isGameOver;

enum PlatformType { NORMAL, MOVING, DISAPPEARING, BOUNCY };

struct Portal {
    SDL_Rect rect;
    bool isActive = true;
    bool isUsed = false;
};

struct Platform {
    SDL_Rect rect;
    PlatformType type;
    bool isVisible = true;
    int direction = 1;
};

struct Enemy {
    SDL_Rect rect;
    bool isAlive = true;
    float velY = 0;
    bool isJumping = false;
    int baseY;
};

struct Bullet {
    SDL_Rect rect;
    bool isActive = false;
};

extern std::vector<Portal> portals;
extern std::vector<Platform> platforms;
extern std::vector<Enemy> enemies;
extern std::vector<Bullet> bullets;

extern SDL_Texture* gameOverBackground;
extern SDL_Texture* playAgainTexture;
extern SDL_Texture* menuButtonTexture;

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path);
void resetGame();
void renderGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, int finalScore, bool& isGameOver, bool& inMenu, bool& running);
void updateGame(SDL_Renderer* renderer, bool& running, bool& inMenu);
void renderGame(SDL_Renderer* renderer, SDL_Texture* backgroundTexture, SDL_Texture* texLeft, SDL_Texture* texRight,
                SDL_Texture* texLeftJump, SDL_Texture* texRightJump, SDL_Texture* texLeftShoot, SDL_Texture* texRightShoot,
                SDL_Texture* platformTexture, SDL_Texture* bulletTexture, SDL_Texture* enemyTexture, SDL_Texture* layoutTexture,
                SDL_Texture* portalTexture);

#endif
