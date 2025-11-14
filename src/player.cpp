#include "player.hpp"
#include "SDL3/SDL_events.h"
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

void Player::update(double dt) {
    if (direction.up) {
        rect.y -= speed*dt;
    }
    if (direction.down) {
        rect.y += speed*dt;
    }
    if (direction.left) {
        rect.x -= speed*dt;
    }
    if (direction.right) {
        rect.x += speed*dt;
    }

}

void Player::handleEvent(SDL_Event* event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.scancode) {
            case SDL_SCANCODE_W:
                this->direction.up = true;
                break;
            case SDL_SCANCODE_S:
                this->direction.down = true;
                break;
            case SDL_SCANCODE_A:
                this->direction.left = true;
                break;
            case SDL_SCANCODE_D:
                this->direction.right = true;
                break;
            default:
                break;
        }
    } else if (event->type == SDL_EVENT_KEY_UP) {
        switch (event->key.scancode) {
            case SDL_SCANCODE_W:
                this->direction.up = false;
                break;
            case SDL_SCANCODE_S:
                this->direction.down = false;
                break;
            case SDL_SCANCODE_A:
                this->direction.left = false;
                break;
            case SDL_SCANCODE_D:
                this->direction.right = false;
                break;
            default:
                break;
        }
    }
}
