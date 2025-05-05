
#include "Player.h"

Player::Player(int x, int y) {
    rect = {x, y, 50, 50};
    velY = 0;
    jumping = false;
    gravity = 800.0f; // pixels/s²
}

void Player::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE && !jumping) {
        startJump();
    }
}

void Player::startJump() {
    jumping = true;
    velY = -400.0f; // initial jump velocity
}

void Player::update(float deltaTime) {
    if (jumping) {
        velY += gravity * deltaTime;
        rect.y += (int)(velY * deltaTime);
        if (rect.y >= 400) {
            rect.y = 400;
            jumping = false;
            velY = 0;
        }
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
    SDL_RenderFillRect(renderer, &rect);
}

bool Player::isJumping() const {
    return jumping;
}

SDL_Rect Player::getRect() const {
    return rect;
}
