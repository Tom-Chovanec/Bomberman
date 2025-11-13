#include "player.hpp"
#include "SDL3/SDL_scancode.h"
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

void Player::update() {

}

void Player::handleKeyEvent(SDL_Scancode event) {
    switch (event) {
        case SDL_SCANCODE_W:
            this->rect.y--;
            break;
        case SDL_SCANCODE_S:
            this->rect.y++;
            break;
        case SDL_SCANCODE_A:
            this->rect.x--;
            break;
        case SDL_SCANCODE_D:
            this->rect.x++;
            break;
        default:
            break;
    }
}
