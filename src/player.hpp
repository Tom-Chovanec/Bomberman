#pragma once
#include "level.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

class Player {
private:
    struct Directions {
        bool up;
        bool down;
        bool left;
        bool right;
    } direction;

    int speed = 100;
    int health = 3;
    SDL_FRect rect = {0, 0, 20, 20}; 
    SDL_FRect spriteRect = {-5, -21, 29, 40}; // relative to rect
    void updateSpriteRect();

public:
    int getHealth();
    void setHealth(int health);

    void update(double dt, LevelManager& lm);
    void handleEvent(SDL_Event* event);

    SDL_FRect& getRect();
    void setRect(SDL_FRect rect);

    SDL_FRect& getSpriteRect();
};
