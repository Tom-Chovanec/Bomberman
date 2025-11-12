#include "player.hpp"
#include <SDL3/SDL_rect.h>

int Player::getHealth() {
    return this->health;
}

void Player::setHealth(int health) {
    this->health = health;
}

SDL_Point Player::getPosition() {
    return this->position;
}

void Player::setPosition(SDL_Point position) {
    this->position = position;
}

Player::~Player() {
    SDL_LogInfo(0, "Player destructed");
}
