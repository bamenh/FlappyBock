#pragma once
#include "CommonFunc.h"


enum GameState {
    MENU,
    PLAY,
    EXIT,
    GAME_OVER
};

void renderMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void renderGamerOver(SDL_Renderer* renderer, SDL_Texture* gameOverTexture, TTF_Font* font, int score) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, gameOverTexture, NULL, NULL);
    renderScore(renderer, font, score);
    SDL_RenderPresent(renderer);
}