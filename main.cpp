#include "game.h"
#include "audio.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    initAudio();

    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cout << "Không thể load font! " << TTF_GetError() << std::endl;
        return -1;
    }

    srand(static_cast<unsigned int>(time(0)));

    SDL_Window* window = SDL_CreateWindow("Doodle jump replica",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* backgroundTexture = loadTexture(renderer, "background1.png");
    SDL_Texture* texLeft = loadTexture(renderer, "left1.png");
    SDL_Texture* texLeftJump = loadTexture(renderer, "left_jump1.png");
    SDL_Texture* texRight = loadTexture(renderer, "right1.png");
    SDL_Texture* texRightJump = loadTexture(renderer, "right_jump1.png");
    SDL_Texture* texLeftShoot = loadTexture(renderer, "shoot.png");
    SDL_Texture* texRightShoot = loadTexture(renderer, "shoot.png");
    SDL_Texture* platformTexture = loadTexture(renderer, "grass.png");
    SDL_Texture* bulletTexture = loadTexture(renderer, "bullet.png");
    SDL_Texture* enemyTexture = loadTexture(renderer, "monter.png");
    SDL_Texture* layoutTexture = loadTexture(renderer, "layout.png");
    SDL_Texture* portalTexture = loadTexture(renderer, "jump_up.png");
    SDL_Texture* menuBackground = IMG_LoadTexture(renderer, "menu.png");
    gameOverBackground = IMG_LoadTexture(renderer, "back1.png");
    playAgainTexture = IMG_LoadTexture(renderer, "playagain.png");
    menuButtonTexture = IMG_LoadTexture(renderer, "bottommenu.png");
    SDL_Texture* playButtonTexture = IMG_LoadTexture(renderer, "playbotton.png");

    if (!menuBackground || !playButtonTexture || !playAgainTexture || !menuButtonTexture) {
        std::cout << "upload fail " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Rect playButtonRect = { SCREEN_WIDTH / 2 - 111, SCREEN_HEIGHT / 2, 222, 80 };

    bool running = true;
    bool inMenu = true;

    Platform startPlat; // platform dau
    startPlat.rect = { player.x - 25, player.y + PLAYER_HEIGHT + 10, 80, 15 };
    startPlat.type = NORMAL;
    platforms.push_back(startPlat);

    int currentY = startPlat.rect.y;
    int lastX = startPlat.rect.x;

    for (int i = 0; i < 20; ++i) {   // tao 20 platform dau
        Platform plat;
        int deltaY = 30 + rand() % 50;
        currentY -= deltaY;
        int deltaX = rand() % 400 - 200;
        int newX = std::max(0, std::min(SCREEN_WIDTH - 80, lastX + deltaX));
        plat.rect = { newX, currentY, 80, 15 };
        plat.type = (rand() % 10 < 3) ? MOVING : NORMAL;
        plat.direction = (rand() % 2 == 0) ? 1 : -1;
        platforms.push_back(plat);
        lastX = newX;

        if (rand() % 100 < 5 && plat.type == NORMAL) { // tao quai
            Enemy e;
            e.rect = { plat.rect.x + plat.rect.w / 2 - 20, plat.rect.y - 40, 40, 40 };
            e.baseY = e.rect.y;
            enemies.push_back(e);
        }
        if (rand() % 100 < 5) {
        Portal p;
        p.rect = { plat.rect.x + plat.rect.w / 2 - 20, plat.rect.y - 40, 40, 40 }; // Đáy portal chạm đỉnh platform
        portals.push_back(p);
    }
    }

    SDL_Event event;

    while (inMenu && running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                inMenu = false;
                running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                if (mouseX >= playButtonRect.x && mouseX <= playButtonRect.x + playButtonRect.w &&
                    mouseY >= playButtonRect.y && mouseY <= playButtonRect.y + playButtonRect.h) {
                    inMenu = false;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBackground, NULL, NULL);
        SDL_RenderCopy(renderer, playButtonTexture, NULL, &playButtonRect);
        SDL_RenderPresent(renderer);
    }

    while (running) {
        if (isGameOver) {
            renderGameOverScreen(renderer, font, score, isGameOver, inMenu, running);
            SDL_Delay(16);
            if (inMenu) {
                while (inMenu) {
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            inMenu = false;
                            running = false;
                        }
                        if (event.type == SDL_MOUSEBUTTONDOWN) {
                            int mouseX = event.button.x;
                            int mouseY = event.button.y;
                            if (mouseX >= playButtonRect.x && mouseX <= playButtonRect.x + playButtonRect.w &&
                                mouseY >= playButtonRect.y && mouseY <= playButtonRect.y + playButtonRect.h) {
                                resetGame();
                                inMenu = false;
                            }
                        }
                    }
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, menuBackground, NULL, NULL);
                    SDL_RenderCopy(renderer, playButtonTexture, NULL, &playButtonRect);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(16);
                }
            }
            continue;
        }

        updateGame(renderer, running, inMenu);
        renderGame(renderer, backgroundTexture, texLeft, texRight, texLeftJump, texRightJump, texLeftShoot, texRightShoot,
                   platformTexture, bulletTexture, enemyTexture, layoutTexture, portalTexture);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texLeft);
    SDL_DestroyTexture(texLeftJump);
    SDL_DestroyTexture(texRight);
    SDL_DestroyTexture(texRightJump);
    SDL_DestroyTexture(texLeftShoot);
    SDL_DestroyTexture(texRightShoot);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(platformTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(portalTexture);
    SDL_DestroyTexture(layoutTexture);
    SDL_DestroyTexture(gameOverBackground);
    SDL_DestroyTexture(playAgainTexture);
    SDL_DestroyTexture(menuButtonTexture);
    SDL_DestroyTexture(playButtonTexture);
    SDL_DestroyTexture(menuBackground);

    freeAudio();
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
