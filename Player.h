#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

class Player {
public:
    Player(int x, int y);
    void handleEvent(SDL_Event& e);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void startJump();
    bool isJumping() const;
    SDL_Rect getRect() const;

private:
    SDL_Rect rect;
    float velY;
    bool jumping;
    float gravity;
};

#endif

