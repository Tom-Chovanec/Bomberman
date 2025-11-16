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
    std::string activeLevel = "";

public:
    ~LevelManager();

    void setActiveLevel(std::string_view name);
    std::string_view getActiveLevel();
    void clearActiveLevel();
    void addLevel(std::string_view name, Level level);
    void loadLevel(std::string_view name, std::string_view levelPath);
    bool checkCollision(SDL_FRect& rect);


    void renderLevel(SDL_Renderer* renderer, TextureManager* tm, std::string_view levelName);

};

