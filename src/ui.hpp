#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <string>

class UI {
private:
    SDL_FRect startButtonRect;
    bool isHovered;
    std::function<void()> onStartGame;

    TTF_Font* titleFont;
    TTF_Font* buttonFont;

    SDL_Texture* titleTexture;
    SDL_Texture* buttonTexture;
    float titleW, titleH;
    float btnTextW, btnTextH;

    SDL_Texture* createTextTexture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, float& w, float& h);

public:
    UI(SDL_Renderer* renderer);
    ~UI();

    void setOnStartGame(std::function<void()> callback);
    void handleEvent(SDL_Event* event);
    void render(SDL_Renderer* renderer);
};
