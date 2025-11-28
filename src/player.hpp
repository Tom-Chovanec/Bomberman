#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include "powerup.hpp"
#include <array>
#include <chrono>

class LevelManager;

class Player {
private:
    struct Directions {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool bomb = false;
    } action;

    std::vector<Powerup> powerups;

    std::array<SDL_Scancode, 5> scancodes;

    int id;

    int baseSpeed = 70;
    int baseMaxBombs = 1;
    int baseRange = 1;

    int health = 3;
    int bombs = 0;
    bool alive = false;
    SDL_FRect rect = {3, 3, 14, 14}; 
    SDL_FRect spriteRect = {-8, -24, 29, 40}; // relative to rect
    void updateSpriteRect();

public:
    Player(int id, const std::array<SDL_Scancode, 5>& scancodes);

    void summon(const SDL_FPoint& pos);

    int getHealth();
    void setHealth(int health);

    void update(std::chrono::milliseconds dt, LevelManager& lm);
    void handleEvent(SDL_Event* event);

    SDL_FRect& getRect();
    void setRect(SDL_FRect rect);

    SDL_FRect& getSpriteRect();

    bool isAlive();

    void addPowerup(Powerup& powerup);

    void reset();
    int getId();
};
