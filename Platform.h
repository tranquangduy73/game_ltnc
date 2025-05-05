#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>

class Platform {
public:
    Platform(int x, int y, int w);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    SDL_Rect getRect() const;

private:
    SDL_Rect rect;
    float speed;
};

#endif

