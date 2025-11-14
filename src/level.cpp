#include "level.hpp"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include <format>
#include <algorithm>

void LevelManager::addLevel(std::string_view name, Level level) {
    levels[name] = level;
}

void LevelManager::loadLevel(std::string_view name, std::string_view levelPath) {
    std::string fullPath = std::format("levels/{}", levelPath);
    std::string data = static_cast<char*>(SDL_LoadFile(fullPath.data(), nullptr));
    int width = data.find('\n');
    int height = std::ranges::count(data, '\n');

    std::erase(data, '\n');
    addLevel(name, Level{.width=width, .height=height, .layout=data});
}

void LevelManager::setActiveLevel(std::string_view name) {
    activeLevel = name;
}

std::string_view LevelManager::getActiveLevel() {
    return activeLevel;
}

void LevelManager::clearActiveLevel() {
    activeLevel = "";
}

LevelManager::~LevelManager() {
    activeLevel = "";
}

void LevelManager::renderLevel(SDL_Renderer* renderer, TextureManager* tm, std::string_view levelName) {
    if (levelName == "") return;

    Level level = levels[levelName];
    if (level.width * level.height != level.layout.length()) {
        SDL_LogWarn(0, "Unable to render level, improper size");
        return;
    }

    int xIdx = 0;
    int yIdx = 0;
    for (char tile : level.layout) {
        SDL_FRect tileRect = {
            .x = static_cast<float>(400.0 / level.width) * xIdx,
            .y = static_cast<float>(300.0 / level.height) * yIdx,
            .w = static_cast<float>(400.0 / level.width),
            .h = static_cast<float>(300.0 / level.height),
        };
        switch (tile) {
        case '0':
            SDL_RenderTexture(renderer, tm->get("ground"), nullptr, &tileRect);
            break;
        case 'X':
            SDL_RenderTexture(renderer, tm->get("wall"), nullptr, &tileRect);
            break;
        }
        xIdx++;
        if (xIdx ==level.width) {
            xIdx = 0;
            yIdx++;
        }
    }
}
