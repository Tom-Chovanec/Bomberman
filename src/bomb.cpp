#include "bomb.hpp"

Bomb::Bomb(int ownerId, const SDL_FRect& rect, int explosionRange, std::chrono::milliseconds ttl)
    : ownerId(ownerId),
      rect(rect),
      explosionRange(explosionRange),
      ttl(ttl),
      exploded(false) {
}

void Bomb::update(std::chrono::milliseconds dt) {
    if (exploded) return;

    if (ttl <= dt) {
        explode();
    } else {
        ttl -= dt;
    }
}

void Bomb::explode() {
    exploded = true;
}

bool Bomb::isExploded() const {
    return exploded;
}

SDL_FRect& Bomb::getRect() {
    return rect;
}

int Bomb::getOwnerId() {
    return ownerId;
}

int Bomb::getExplosionRange() const {
    return explosionRange;
}
