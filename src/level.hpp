#pragma once

#include "SDL3/SDL_render.h"
#include "texture_manager.hpp"
#include <memory>
#include <string_view>
#include <string>
#include <unordered_map>
#include <random>
#include <vector>


struct Level {
    int width;
    int height;
    std::string layout;
    std::vector<SDL_FRect> spawnZones;
};

class LevelManager {
private:
    std::unordered_map<std::string_view, Level> levels;
    std::unique_ptr<Level> activeLevel = nullptr;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> distrib;

public:
    LevelManager();
    ~LevelManager();

    void setActiveLevel(std::string_view levelName);
    Level& getActiveLevel();
    void clearActiveLevel();
    void addLevel(std::string_view name, Level level);
    void loadLevel(std::string_view name, std::string_view levelPath);
    Level getPupulatedLevel(std::string_view levelName);
    bool checkCollision(SDL_FRect& rect);

    void placeBomb(const SDL_FRect& rect);

    void update();

    void renderLevel(SDL_Renderer* renderer, TextureManager* tm) const;

};

