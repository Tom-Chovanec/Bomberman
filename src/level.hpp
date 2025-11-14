#pragma once

#include "SDL3/SDL_render.h"
#include "texture_manager.hpp"
#include <string_view>
#include <string>
#include <unordered_map>


struct Level {
    int width;
    int height;
    std::string layout;
};

class LevelManager {
private:
    std::unordered_map<std::string_view, Level> levels;
    Level activeLevel;

public:
    ~LevelManager();

    void addLevel(std::string_view name, Level level);


    void renderLevel(SDL_Renderer* renderer, TextureManager* tm, std::string_view levelName);

};

