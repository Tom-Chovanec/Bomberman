#pragma once
#include <SDL3/SDL.h>
#include <string_view>
#include <unordered_map>

class TextureManager {
private:
    std::unordered_map<std::string_view, SDL_Texture*> textures;

public:
    ~TextureManager();

    bool load(SDL_Renderer* renderer, std::string_view id, std::string_view path);
    SDL_Texture* get(std::string_view id);
};
