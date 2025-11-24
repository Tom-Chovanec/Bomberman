#pragma once
#include "SDL3/SDL_rect.h"
#include <chrono>

class Bomb {
private:
    int ownerId;
    int explosionRange;
    SDL_FRect rect;
    std::chrono::milliseconds ttl;
    bool exploded;

public:
    Bomb(int ownerId, const SDL_FRect& rect, int explosionRange, std::chrono::milliseconds ttl);

    void update(std::chrono::milliseconds dt);
    void explode();

    bool isExploded() const;

    int getOwnerId();
    int getExplosionRange() const ;

    SDL_FRect& getRect();

};
