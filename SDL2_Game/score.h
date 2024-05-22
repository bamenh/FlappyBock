#pragma once
#include "CommonFunc.h"

bool isNight(int score) {
    if ((score/100) % 2 == 1) {
        return true;
    }
    else {
        return false;
    }
}

void renderScore(SDL_Renderer* renderer, TTF_Font* font, int score) {
    SDL_Color textColor = { 255, 255, 255, 255 }; // white color
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { 30, 20, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

