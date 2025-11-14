#pragma once
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
    SDL_FRect rect = {0, 0, 29, 40};

public:
    int getHealth();
    void setHealth(int health);

    void update(double dt);
    void handleEvent(SDL_Event* event);

    SDL_FRect getRect();
    void setRect(SDL_FRect rect);
};
