#include "game.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "audio.h"

int score = 0;
float velY = 0;
int highestY = SCREEN_HEIGHT;
TTF_Font* font = nullptr;
SDL_Color textColor = {255, 255, 255};
SDL_Rect player = { SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2, SCREEN_HEIGHT - 150, PLAYER_WIDTH, PLAYER_HEIGHT };

SDL_Rect playAgainRect = { SCREEN_WIDTH / 2 - 111, 300, 222, 80 };
SDL_Rect menuRect = { SCREEN_WIDTH / 2 - 111, 400, 222, 80 };
bool isGameOver = false;

std::vector<Portal> portals;
std::vector<Platform> platforms;
std::vector<Enemy> enemies;
std::vector<Bullet> bullets;

SDL_Texture* gameOverBackground = NULL;
SDL_Texture* playAgainTexture = NULL;
SDL_Texture* menuButtonTexture = NULL;

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void resetGame() {
    player.x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    player.y = SCREEN_HEIGHT - 150;
    velY = 0;
    score = 0;
    highestY = SCREEN_HEIGHT;
    platforms.clear();
    enemies.clear();
    bullets.clear();
    portals.clear();

    Platform startPlat;
    startPlat.rect = { player.x - 25, player.y + PLAYER_HEIGHT + 10, 80, 15 };
    startPlat.type = NORMAL;
    platforms.push_back(startPlat);
}

void renderGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, int finalScore, bool& isGameOver, bool& inMenu, bool& running) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, gameOverBackground, NULL, NULL);

    SDL_Color textColor = {0, 0, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game Over", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, 200, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    std::string scoreText = "Your Score: " + std::to_string(finalScore);
    textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect scoreRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, 150, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &scoreRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    SDL_RenderCopy(renderer, playAgainTexture, NULL, &playAgainRect);
    SDL_RenderCopy(renderer, menuButtonTexture, NULL, &menuRect);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            isGameOver = false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = event.button.x;
            int mouseY = event.button.y;

            if (mouseX >= playAgainRect.x && mouseX <= playAgainRect.x + playAgainRect.w &&
                mouseY >= playAgainRect.y && mouseY <= playAgainRect.y + playAgainRect.h) {
                resetGame();
                isGameOver = false;
                inMenu = false;
            }
            else if (mouseX >= menuRect.x && mouseX <= menuRect.x + menuRect.w &&
                     mouseY >= menuRect.y && mouseY <= menuRect.y + menuRect.h) {
                isGameOver = false;
                inMenu = true;
            }
        }
    }
}

void updateGame(SDL_Renderer* renderer, bool& running, bool& inMenu) {
    static bool facingRight = true;
    static bool isJumping = false;
    static bool isShooting = false;
    static Uint32 lastBulletTime = 0;
    static Uint32 shootAnimationTime = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_LEFT]) {
        player.x -= MOVE_SPEED;
        facingRight = false;
    }
    if (keystates[SDL_SCANCODE_RIGHT]) {
        player.x += MOVE_SPEED;
        facingRight = true;
    }
    if (keystates[SDL_SCANCODE_SPACE]) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastBulletTime >= BULLET_COOLDOWN) {
            Bullet newBullet;
            newBullet.rect = { player.x + PLAYER_WIDTH / 2 - 10, player.y - 40, 20, 40 };
            newBullet.isActive = true;
            bullets.push_back(newBullet);
            lastBulletTime = currentTime;
            isShooting = true;
            shootAnimationTime = currentTime;
            playShootSound();
        }
    }

    if (isShooting && SDL_GetTicks() - shootAnimationTime > 200) {
        isShooting = false;
    }

    if (player.x + PLAYER_WIDTH < 0) player.x = SCREEN_WIDTH;
    if (player.x > SCREEN_WIDTH) player.x = -PLAYER_WIDTH;

    player.y += static_cast<int>(velY);
    velY += GRAVITY;
    isJumping = velY < 0;

    if (player.y < SCREEN_HEIGHT / 2) {
        int dy = SCREEN_HEIGHT / 2 - player.y;  //cuon man
        player.y = SCREEN_HEIGHT / 2;
        for (auto& plat : platforms) plat.rect.y += dy;
        for (auto& bullet : bullets) bullet.rect.y += dy;
        for (auto& e : enemies) {
            e.rect.y += dy;
            e.baseY += dy;
        }
        for (auto& p : portals) p.rect.y += dy;
    }

    for (auto& e : enemies) {
        if (e.isAlive) {
            if (!e.isJumping && rand() % 100 < 10) {
                e.velY = -ENEMY_JUMP_SPEED;
                e.isJumping = true;
            }
            e.rect.y += static_cast<int>(e.velY);
            e.velY += GRAVITY * 0.5f;
            if (e.rect.y >= e.baseY) {
                e.rect.y = e.baseY;
                e.velY = 0;
                e.isJumping = false;
            }
            if (e.rect.y < e.baseY - ENEMY_JUMP_HEIGHT) {
                e.velY = ENEMY_JUMP_SPEED;
            }
        }
    }

    for (auto& plat : platforms) { // platform
        if (plat.type == MOVING) {
            plat.rect.x += MOVING_PLATFORM_SPEED * plat.direction;
            if (plat.rect.x <= 0) {
                plat.rect.x = 0;
                plat.direction = 1;
            } else if (plat.rect.x + plat.rect.w >= SCREEN_WIDTH) {
                plat.rect.x = SCREEN_WIDTH - plat.rect.w;
                plat.direction = -1;
            }
        }
    }

    bool landedOnPlatform = false; // check cham platform
    for (auto& plat : platforms) {
        if (velY > 0 && player.y + PLAYER_HEIGHT <= plat.rect.y + velY &&
            player.y + PLAYER_HEIGHT >= plat.rect.y &&
            player.x + PLAYER_WIDTH > plat.rect.x &&
            player.x < plat.rect.x + plat.rect.w) {
            velY = JUMP_VELOCITY;
            isJumping = true;
            player.y = plat.rect.y - PLAYER_HEIGHT;
            if (!landedOnPlatform) {
                score += 10;
                landedOnPlatform = true;
                playLandSound();
            }
            if (plat.type == MOVING) {
                player.x += MOVING_PLATFORM_SPEED * plat.direction;
                if (player.x < 0) player.x = 0;
                if (player.x + PLAYER_WIDTH > SCREEN_WIDTH) player.x = SCREEN_WIDTH - PLAYER_WIDTH;
            }
        }
    }

    for (auto& e : enemies) {  // check va cham enemies
        if (e.isAlive && SDL_HasIntersection(&player, &e.rect)) {
            if (velY > 0 && player.y + PLAYER_HEIGHT - velY <= e.rect.y &&
                player.x + PLAYER_WIDTH > e.rect.x && player.x < e.rect.x + e.rect.w) {
                e.isAlive = false;
                e.velY = 5;
                velY = JUMP_VELOCITY;
                score += 50;
                playEnemyHitSound();
            } else {
                isGameOver = true;
            }
        }
    }

    for (auto& bullet : bullets) { // va cham dan quai
    if (bullet.isActive) {
        bullet.rect.y -= BULLET_SPEED;
        if (bullet.rect.y + bullet.rect.h < 0) {
            bullet.isActive = false;
        } else {
            for (auto it = enemies.begin(); it != enemies.end(); ) {
                if (it->isAlive && SDL_HasIntersection(&bullet.rect, &it->rect)) {
                    bullet.isActive = false;
                    it = enemies.erase(it); // Xóa quái vật ngay lập tức
                    score += 50;
                    playEnemyHitSound();
                    break;
                } else {
                    ++it;
                }
            }
        }
    }
}

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.isActive; }), bullets.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return (!e.isAlive && e.rect.y > SCREEN_HEIGHT) || e.rect.y > SCREEN_HEIGHT; }), enemies.end());

    for (auto& portal : portals) {//
        if (portal.isActive && !portal.isUsed && SDL_HasIntersection(&player, &portal.rect)) {
            Portal* targetPortal = nullptr;
            for (auto& p : portals) {
                if (&p != &portal && p.isActive && p.rect.y < portal.rect.y) {
                    targetPortal = &p;
                    break;
                }
            }
            if (targetPortal) {
                player.x = targetPortal->rect.x + targetPortal->rect.w / 2 - PLAYER_WIDTH / 2;
                player.y = targetPortal->rect.y - PLAYER_HEIGHT;
                velY = 0;
                portal.isUsed = true;
                targetPortal->isActive = false;
                score += 20;
            }
            break;
        }
    }
    portals.erase(std::remove_if(portals.begin(), portals.end(),
        [](const Portal& p) { return p.rect.y > SCREEN_HEIGHT; }), portals.end());

    if (!platforms.empty() && platforms.back().rect.y > 0) {
        Platform newPlat;
        newPlat.rect.w = 80;
        newPlat.rect.h = 15;
        newPlat.rect.y = platforms.back().rect.y - (30 + rand() % 50);
        newPlat.rect.x = std::max(0, std::min(SCREEN_WIDTH - 80, platforms.back().rect.x + (rand() % 400 - 200)));
        newPlat.type = (rand() % 10 < 3) ? MOVING : NORMAL;
        newPlat.direction = (rand() % 2 == 0) ? 1 : -1;
        platforms.push_back(newPlat);

        if (rand() % 100 < 5 && newPlat.type == NORMAL) {
            Enemy e;
            e.rect = { newPlat.rect.x + newPlat.rect.w / 2 - 20, newPlat.rect.y - 40, 40, 40 };
            e.baseY = e.rect.y;
            enemies.push_back(e);
        }
        if (rand() % 100 < 5) {
            Portal p;
            p.rect = { newPlat.rect.x + newPlat.rect.w / 2 - 20, newPlat.rect.y - 40, 40, 40 };
            portals.push_back(p);
        }
    }

    platforms.erase(std::remove_if(platforms.begin(), platforms.end(),
        [](Platform& p) { return p.rect.y > SCREEN_HEIGHT; }), platforms.end());

    if (player.y > SCREEN_HEIGHT) {
        isGameOver = true;
        playFallSound();
    }
}

void renderGame(SDL_Renderer* renderer, SDL_Texture* backgroundTexture, SDL_Texture* texLeft, SDL_Texture* texRight,
                SDL_Texture* texLeftJump, SDL_Texture* texRightJump, SDL_Texture* texLeftShoot, SDL_Texture* texRightShoot,
                SDL_Texture* platformTexture, SDL_Texture* bulletTexture, SDL_Texture* enemyTexture, SDL_Texture* layoutTexture,
                SDL_Texture* portalTexture) {
    static bool facingRight = true;
    static bool isJumping = false;
    static bool isShooting = false;
    static Uint32 shootAnimationTime = 0;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    for (auto& plat : platforms) {
        SDL_RenderCopy(renderer, platformTexture, NULL, &plat.rect);
    }
    for (auto& bullet : bullets) {
        SDL_Rect destRect = {
            bullet.rect.x, bullet.rect.y,
            int(bullet.rect.w * BULLET_SCALE),
            int(bullet.rect.h * BULLET_SCALE)
        };
        SDL_RenderCopy(renderer, bulletTexture, NULL, &destRect);
    }
    for (auto& e : enemies) {
        SDL_RenderCopy(renderer, enemyTexture, NULL, &e.rect);
    }
    for (auto& portal : portals) {
        if (portal.isActive) {
            SDL_RenderCopy(renderer, portalTexture, NULL, &portal.rect);
        }
    }

    SDL_Texture* currentTexture;
    if (isShooting && SDL_GetTicks() - shootAnimationTime <= 200) {
        currentTexture = facingRight ? texRightShoot : texLeftShoot;
    } else {
        currentTexture = facingRight ?
            (isJumping ? texRightJump : texRight) :
            (isJumping ? texLeftJump : texLeft);
    }
    SDL_Rect dstRect = { player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT };
    SDL_RenderCopy(renderer, currentTexture, NULL, &dstRect);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    SDL_Rect layoutRect = { 0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50 };
    SDL_RenderCopy(renderer, layoutTexture, NULL, &layoutRect);

    SDL_RenderPresent(renderer);
}
