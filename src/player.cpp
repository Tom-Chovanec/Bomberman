#include "player.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_scancode.h"
#include <SDL3/SDL_rect.h>

Player::Player(const std::array<SDL_Scancode, 5>& scancodes) {
    this->scancodes = scancodes;
}

void Player::summon(const SDL_FPoint& pos) {
    health = 3;
    alive = true;
    rect.x = pos.x;
    rect.y = pos.y;
}


int Player::getHealth() {
    return health;
}

void Player::setHealth(int health) {
    this->health = health;
}

SDL_FRect& Player::getRect() {
    return rect;
}

SDL_FRect& Player::getSpriteRect() {
    return spriteRect;
}

void Player::setRect(SDL_FRect rect) {
    this->rect = rect;
}

void Player::updateSpriteRect() {
    spriteRect.x = rect.x - 5;
    spriteRect.y = rect.y - 21;
}

void Player::update(double dt, LevelManager& lm) {
    if (!alive) return;

    if (health == 0) {
        alive = false;
        return;
    }

    if (action.up) {
        rect.y -= speed*dt;
        if (lm.checkCollision(rect)) {
            rect.y += speed*dt;
        }
    }

    if (action.down) {
        rect.y += speed*dt;
        if (lm.checkCollision(rect)) {
            rect.y -= speed*dt;
        }
    }

    if (action.left) {
        rect.x -= speed*dt;
        if (lm.checkCollision(rect)) {
            rect.x += speed*dt;
        }
    }

    if (action.right) {
        rect.x += speed*dt;
        if (lm.checkCollision(rect)) {
            rect.x -= speed*dt;
        }
    }

    if (action.bomb) {
        lm.placeBomb(rect);
    }

    updateSpriteRect();
}

void Player::handleEvent(SDL_Event* event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Scancode scancode = event->key.scancode;
        if (scancode == scancodes[0])
            action.up = true;
        else if (scancode == scancodes[1])
            action.down = true;
        else if (scancode == scancodes[2])
            action.left = true;
        else if (scancode == scancodes[3])
            action.right = true;
        else if (scancode == scancodes[4])
            action.bomb = true;
    } else if (event->type == SDL_EVENT_KEY_UP) {
        SDL_Scancode scancode = event->key.scancode;
        if (scancode == scancodes[0])
            action.up = false;
        else if (scancode == scancodes[1])
            action.down = false;
        else if (scancode == scancodes[2])
            action.left = false;
        else if (scancode == scancodes[3])
            action.right = false;
        else if (scancode == scancodes[4])
            action.bomb = false;
    }
}

bool Player::isAlive() {
    return alive;
}
