#include "level.hpp"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
#include "bomb.hpp"
#include "player.hpp"
#include "powerup.hpp"
#include <array>
#include <chrono>
#include <format>
#include <algorithm>
#include <memory>

LevelManager::LevelManager() :
    gen(rd()),
    distrib(1, 100)
{
}

void LevelManager::addLevel(std::string_view name, Level level) {
    levels[name] = level;
}

void LevelManager::loadLevel(std::string_view name, std::string_view levelPath) { std::string fullPath = std::format("levels/{}", levelPath);
    std::string data = static_cast<char*>(SDL_LoadFile(fullPath.data(), nullptr));
    int width = data.find('\n');
    int height = std::ranges::count(data, '\n');

    std::erase(data, '\n');
    addLevel(name, Level{.width=width, .height=height, .layout=data});
}

Level& LevelManager::getActiveLevel() {
    return *activeLevel;
}

void LevelManager::clearActiveLevel() {
    activeLevel = nullptr;
}


Level LevelManager::getPupulatedLevel(std::string_view levelName) {
    if (levelName == "") return Level();

    Level level = levels[levelName];

    int xIdx = 0;
    int yIdx = 0;
    for (char& tile : level.layout) {
        switch (tile) {
            case ('0'):
                if (distrib(gen) > 10) {
                    tile = 'C';
                }
                break;
            case ('S'): {
                SDL_FRect tileRect = {
                    .x = static_cast<float>(400.0 / level.width) * xIdx,
                    .y = static_cast<float>(300.0 / level.height) * yIdx,
                    .w = static_cast<float>(400.0 / level.width),
                    .h = static_cast<float>(300.0 / level.height),
                };
                level.spawnZones.push_back(tileRect);
            }
        }
        xIdx++;
        if (xIdx == level.width) {
            xIdx = 0;
            yIdx++;
        }
    }

    return level;
}

void LevelManager::setActiveLevel(std::string_view name) {
    Level level = getPupulatedLevel(name);
    activeLevel = std::make_unique<Level>(level);
}

LevelManager::~LevelManager() {
    activeLevel = nullptr;
}

void LevelManager::renderLevel(SDL_Renderer* renderer, TextureManager* tm) const {
    if (activeLevel == nullptr) return;

    if (activeLevel->width * activeLevel->height != activeLevel->layout.length()) {
        SDL_LogWarn(0, "Unable to render activeLevel-> improper size");
        return;
    }

    int xIdx = 0;
    int yIdx = 0;
    for (char tile : activeLevel->layout) {
        SDL_FRect tileRect = {
            .x = static_cast<float>(400.0 / activeLevel->width) * xIdx,
            .y = static_cast<float>(300.0 / activeLevel->height) * yIdx,
            .w = static_cast<float>(400.0 / activeLevel->width),
            .h = static_cast<float>(300.0 / activeLevel->height),
        };
        switch (tile) {
        case '0':
            SDL_RenderTexture(renderer, tm->get("ground"), nullptr, &tileRect);
            break;
        case 'S':
            SDL_RenderTexture(renderer, tm->get("ground"), nullptr, &tileRect);
            break;
        case 'E':
            SDL_RenderTexture(renderer, tm->get("ground"), nullptr, &tileRect);
            break;
        case 'X':
            SDL_RenderTexture(renderer, tm->get("wall"), nullptr, &tileRect);
            break;
        case 'C':
            SDL_RenderTexture(renderer, tm->get("crate"), nullptr, &tileRect);
            break;
        }
        xIdx++;
        if (xIdx ==activeLevel->width) {
            xIdx = 0;
            yIdx++;
        }
    }

    for (auto& bomb : bombs) {
        SDL_RenderTexture(renderer, tm->get("bomb"), nullptr, &bomb->getRect());
    }

    for (auto& flame : flames) {
        SDL_RenderTexture(renderer, tm->get("flame"), nullptr, &flame.rect);
    }

    for (auto& powerup : powerups) {
        switch (powerup.powerup.getType()) {
        case PowerupType::FIRE_UP:
            SDL_RenderTexture(renderer, tm->get("fire_up_upgrade"), nullptr, &powerup.rect);
            break;
        case PowerupType::BOMB_UP:
            SDL_RenderTexture(renderer, tm->get("bomb_up_upgrade"), nullptr, &powerup.rect);
            break;
        case PowerupType::SKATE:
            SDL_RenderTexture(renderer, tm->get("skate_upgrade"), nullptr, &powerup.rect);
            break;
        }
    }
}

bool LevelManager::placeBomb(int ownerId, const SDL_FRect& rect, int range) {
    float maxOverlapArea = 0.0f;
    float targetX = -1;
    float targetY = -1;

    float tileW = 400.0f / activeLevel->width;
    float tileH = 300.0f / activeLevel->height;

    for (int y = 0; y < activeLevel->height; ++y) {
        for (int x = 0; x < activeLevel->width; ++x) {
            SDL_FRect tileRect = {
                .x = static_cast<float>(x) * tileW,
                .y = static_cast<float>(y) * tileH,
                .w = tileW,
                .h = tileH
            };

            SDL_FRect intersection;
            if (SDL_GetRectIntersectionFloat(&tileRect, &rect, &intersection)) {
                float area = intersection.w * intersection.h;

                if (area > maxOverlapArea) {
                    maxOverlapArea = area;
                    targetX = tileRect.x;
                    targetY = tileRect.y;
                }
            }
        }
    }

    bool bombExists = std::any_of(
        bombs.begin(),
        bombs.end(),
        [&](const std::unique_ptr<Bomb>& bomb) {
            const SDL_FRect& bRect = bomb->getRect();
            return fabs(bRect.x - targetX) < 0.001f &&
                   fabs(bRect.y - targetY) < 0.001f;
        }
    );

    if (targetX != -1 && targetY != -1 && !bombExists) {
        bombs.emplace_back(std::make_unique<Bomb>(ownerId, SDL_FRect{targetX, targetY, tileW, tileH}, range, std::chrono::milliseconds(1000)));
        return true;
    }
    return false;
}

void LevelManager::update(std::chrono::milliseconds dt, std::array<Player*, 2> players) {
    for (auto it = flames.begin(); it != flames.end();) {
        it->ttl -= dt;

        if (it->ttl.count() > 0) {
            for (auto& p : players) {
                if (SDL_HasRectIntersectionFloat(&it->rect, &p->getRect())) {
                    p->setHealth(p->getHealth() - 1);
                }
            }
        }

        if (it->ttl.count() <= 0) {
            it = flames.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = powerups.begin(); it != powerups.end();) {
        bool pickedUp = false;
        for (auto& p : players) {
            if (SDL_HasRectIntersectionFloat(&it->rect, &p->getRect())) {
                p->addPowerup(it->powerup);
                pickedUp = true;
                break;
            }
        }
        if (pickedUp) {
            it = powerups.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = bombs.begin(); it != bombs.end();) {
        auto& bomb = *it;
        bomb->update(dt);

        if (bomb->isExploded()) {
            explodedBombCount[bomb->getOwnerId()]++;

            SDL_FRect bombRect = bomb->getRect();
            int range = bomb->getExplosionRange();
            float tileW = 400.0f / activeLevel->width;
            float tileH = 300.0f / activeLevel->height;

            int startX = static_cast<int>((bombRect.x + tileW / 2) / tileW);
            int startY = static_cast<int>((bombRect.y + tileH / 2) / tileH);

            flames.push_back({bombRect, std::chrono::milliseconds(200)});

            int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

            for (auto& dir : dirs) {
                for (int i = 1; i <= range; ++i) {
                    int cx = startX + (dir[0] * i);
                    int cy = startY + (dir[1] * i);

                    if (cx < 0 || cx >= activeLevel->width || cy < 0 || cy >= activeLevel->height)
                        break;

                    int idx = cy * activeLevel->width + cx;
                    char& tile = activeLevel->layout[idx];

                    SDL_FRect flameRect = {
                        static_cast<float>(cx) * tileW,
                        static_cast<float>(cy) * tileH,
                        tileW,
                        tileH
                    };

                    if (tile == 'X') {
                        break;
                    } else if (tile == 'C') {
                        tile = '0';
                        flames.push_back({flameRect, std::chrono::milliseconds(200)});
                        if (distrib(rd) < 30) {
                            powerups.push_back({getPowerupType(), flameRect});
                        }
                        break;
                    } else {
                        flames.push_back({flameRect, std::chrono::milliseconds(200)});
                    }
                }
            }
            it = bombs.erase(it);
        } else {
            ++it;
        }
    }
}

PowerupType LevelManager::getPowerupType() {
    // distrib is 0 - 100
    int tmp = distrib(rd);
    if (tmp <= 20) {
        return PowerupType::BOMB_UP;
    } else if (tmp <= 60) {
        return PowerupType::FIRE_UP;
    } else {
        return PowerupType::SKATE;
    }
}
bool LevelManager::checkCollision(SDL_FRect& rect) {
    if (activeLevel == nullptr) return false;

    int xIdx = 0;
    int yIdx = 0;
    for (char tile : activeLevel->layout) {
        switch (tile) {
        case 'C':
        case 'X': {
            SDL_FRect tileRect = {
                .x = static_cast<float>(400.0 / activeLevel->width) * xIdx,
                .y = static_cast<float>(300.0 / activeLevel->height) * yIdx,
                .w = static_cast<float>(400.0 / activeLevel->width),
                .h = static_cast<float>(300.0 / activeLevel->height),
            };
            if (SDL_HasRectIntersectionFloat(&tileRect, &rect))
                return true;
            break;
        }}
        xIdx++;
        if (xIdx ==activeLevel->width) {
            xIdx = 0;
            yIdx++;
        }
    }
    return false;
}

int LevelManager::bombsExploded(int ownerId) {
    int tmp =explodedBombCount[ownerId];
    explodedBombCount[ownerId] = 0;
    return tmp;
}

void LevelManager::reset() {
    bombs.clear();
    flames.clear();
    powerups.clear();
}
