#pragma once

#include "SDL3/SDL_render.h"
#include "bomb.hpp"
#include "powerup.hpp"
#include "texture_manager.hpp"
#include <array>
#include <chrono>
#include <memory>
#include <string_view>
#include <string>
#include <unordered_map>
#include <random>
#include <vector>

class Player;

struct Flame {
    SDL_FRect rect;
    std::chrono::milliseconds ttl;
};

struct Level {
    int width;
    int height;
    std::string layout;
    std::vector<SDL_FRect> spawnZones;
};

struct PowerupTile {
    Powerup powerup;
    SDL_FRect rect;
};

class LevelManager {
private:
    std::unordered_map<std::string_view, Level> levels;
    std::unique_ptr<Level> activeLevel = nullptr;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> distrib;

    std::vector<std::unique_ptr<Bomb>> bombs;
    std::vector<Flame> flames;
    std::vector<PowerupTile> powerups;

    std::array<int, 4> explodedBombCount = {0, 0, 0, 0};

    PowerupType getPowerupType();
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

    bool placeBomb(int ownerId, const SDL_FRect& rect, int range);

    void update(std::chrono::milliseconds dt, std::array<Player*, 2> players);

    int bombsExploded(int ownerId);

    void renderLevel(SDL_Renderer* renderer, TextureManager* tm) const;
    void reset();

};

