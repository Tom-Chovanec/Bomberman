#include "level.hpp"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
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
        case 'B':
            SDL_RenderTexture(renderer, tm->get("bomb"), nullptr, &tileRect);
            break;
        }
        xIdx++;
        if (xIdx ==activeLevel->width) {
            xIdx = 0;
            yIdx++;
        }
    }
}

void LevelManager::placeBomb(const SDL_FRect& rect) {
    float maxOverlapArea = 0.0f;
    int targetX = -1;
    int targetY = -1;

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
                    targetX = x;
                    targetY = y;
                }
            }
        }
    }

    if (targetX != -1 && targetY != -1) {
        int tileIndex = targetY * activeLevel->width + targetX;

        activeLevel->layout[tileIndex] = 'B'; 
    }
}

void LevelManager::update() {
    
}

bool LevelManager::checkCollision(SDL_FRect& rect) {
    if (activeLevel == nullptr) return false;

    if (activeLevel->width * activeLevel->height != activeLevel->layout.length()) {
        SDL_LogWarn(0, "Unable to render activeLevel-> improper size");
        return false;
    }

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
