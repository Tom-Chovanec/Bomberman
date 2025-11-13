#include "texture_manager.hpp"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>
#include <string_view>
#include <format>

TextureManager::~TextureManager() {
    for (auto const& [id, texture] : textures) {
        SDL_DestroyTexture(texture);
    }
    textures.clear();
}


bool TextureManager::load(SDL_Renderer* renderer, std::string_view id, std::string_view path) {
    SDL_Surface* surface = nullptr;

    std::string fullpath = std::format("{}assets/{}", SDL_GetBasePath(), path);
    surface = SDL_LoadPNG(fullpath.data());
    if (!surface) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return false;
    }

    textures[id] = SDL_CreateTextureFromSurface(renderer, surface);
    if (!textures[id]) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(textures[id], SDL_SCALEMODE_PIXELART);

    SDL_DestroySurface(surface); 
    return true;
}

SDL_Texture* TextureManager::get(std::string_view id) {
    auto it = textures.find(id);
    if (it != textures.end()){
        return it->second;
    }
    return nullptr;
}
