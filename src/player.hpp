#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

class Player {
private:
    int health = 3;
    SDL_FRect rect = {0, 0, 100, 100};

public:
    ~Player();

    int getHealth();
    void setHealth(int health);

    SDL_FRect getRect();
    void setRect(SDL_FRect rect);
};
