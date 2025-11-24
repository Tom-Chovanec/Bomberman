#pragma once
#include "level.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <array>

class Player {
private:
    struct Directions {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool bomb = false;
    } action;

    std::array<SDL_Scancode, 5> scancodes;

    int speed = 100;
    int health = 3;
    bool alive = false;
    SDL_FRect rect = {0, 0, 20, 20}; 
    SDL_FRect spriteRect = {-5, -21, 29, 40}; // relative to rect
    void updateSpriteRect();

public:
    Player(const std::array<SDL_Scancode, 5>& scancodes);

    void summon(const SDL_FPoint& pos);

    int getHealth();
    void setHealth(int health);

    void update(double dt, LevelManager& lm);
    void handleEvent(SDL_Event* event);

    SDL_FRect& getRect();
    void setRect(SDL_FRect rect);

    SDL_FRect& getSpriteRect();

    bool isAlive();
};
