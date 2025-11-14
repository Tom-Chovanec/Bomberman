#include "level.hpp"
#include "SDL3/SDL_log.h"

void LevelManager::addLevel(std::string_view name, Level level) {
    levels[name] = level;
}

LevelManager::~LevelManager() {
    levels.clear();
}

void LevelManager::renderLevel(SDL_Renderer* renderer, TextureManager* tm, std::string_view levelName) {
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
