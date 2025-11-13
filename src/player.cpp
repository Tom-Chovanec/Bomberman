#include "player.hpp"
#include <SDL3/SDL_rect.h>

int Player::getHealth() {
    return this->health;
}

void Player::setHealth(int health) {
    this->health = health;
}

SDL_FRect Player::getRect() {
    return this->rect;
}

void Player::setRect(SDL_FRect rect) {
    this->rect = rect;
}

Player::~Player() {
    SDL_LogInfo(0, "Player destructed");
}
