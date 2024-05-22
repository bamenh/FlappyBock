#pragma once
#include "CommonFunc.h"

const int OBSTACLE_WIDTH = 120;
const int OBSTACLE_HEIGHT = 150;
const int OBSTACLE_SPEED = 3;
const int GROUND_SPEED = 1;

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w >= b.x &&
        b.x + b.w >= a.x &&
        a.y + a.h >= b.y &&
        b.y + b.h >= a.y) {
        return true;
    }
    return false;
}