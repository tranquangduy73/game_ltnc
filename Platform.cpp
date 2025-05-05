#include "Platform.h"

Platform::Platform(int x, int y, int w) {
    rect = {x, y, w, 20};
    speed = 200.0f; // pixels per second
}

void Platform::update(float deltaTime) {
    rect.x -= (int)(speed * deltaTime);
}

void Platform::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
    SDL_RenderFillRect(renderer, &rect);

}

SDL_Rect Platform::getRect() const {
    return rect;
}

