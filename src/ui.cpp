#include "ui.hpp"

UI::UI(SDL_Renderer* renderer) : isHovered(false), titleFont(nullptr), buttonFont(nullptr), titleTexture(nullptr), buttonTexture(nullptr) {
    float buttonWidth = 200.0f;
    float buttonHeight = 60.0f;
    startButtonRect = { 
        (800.0f - buttonWidth) / 2.0f, 
        (600.0f - buttonHeight) / 2.0f + 60.0f, 
        buttonWidth, 
        buttonHeight 
    };

    titleFont = TTF_OpenFont("assets/Jersey10-Regular.ttf", 128.0f); 
    buttonFont = TTF_OpenFont("assets/Jersey10-Regular.ttf", 32.0f);

    if (!titleFont || !buttonFont) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
    } else {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color red = {255, 60, 60, 255};

        titleTexture = createTextTexture(renderer, titleFont, "BOMBERMAN", red, titleW, titleH);
        buttonTexture = createTextTexture(renderer, buttonFont, "START", white, btnTextW, btnTextH);
    }
}

UI::~UI() {
    if (titleTexture) SDL_DestroyTexture(titleTexture);
    if (buttonTexture) SDL_DestroyTexture(buttonTexture);
    if (titleFont) TTF_CloseFont(titleFont);
    if (buttonFont) TTF_CloseFont(buttonFont);
}

SDL_Texture* UI::createTextTexture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, float& w, float& h) {
    if (!font) return nullptr;

    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (!surf) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    w = (float)surf->w;
    h = (float)surf->h;

    SDL_DestroySurface(surf);
    return tex;
}

void UI::setOnStartGame(std::function<void()> callback) {
    onStartGame = callback;
}

void UI::handleEvent(SDL_Event* event) {
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        SDL_FPoint mousePoint = { event->motion.x, event->motion.y };
        isHovered = SDL_PointInRectFloat(&mousePoint, &startButtonRect);
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->button.button == SDL_BUTTON_LEFT && isHovered) {
            if (onStartGame) {
                onStartGame();
            }
        }
    }
}

void UI::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_FRect screen = {0, 0, 800, 600};
    SDL_RenderFillRect(renderer, &screen);

    if (titleTexture) {
        SDL_FRect titleRect = { (800.0f - titleW) / 2.0f, 100.0f, titleW, titleH };
        SDL_RenderTexture(renderer, titleTexture, nullptr, &titleRect);
    }

    if (isHovered) {
        SDL_SetRenderDrawColor(renderer, 60, 180, 60, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    }
    SDL_RenderFillRect(renderer, &startButtonRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &startButtonRect);

    if (buttonTexture) {
        SDL_FRect textRect = { 
            startButtonRect.x + (startButtonRect.w - btnTextW) / 2.0f, 
            startButtonRect.y + (startButtonRect.h - btnTextH) / 2.0f, 
            btnTextW, 
            btnTextH 
        };
        SDL_RenderTexture(renderer, buttonTexture, nullptr, &textRect);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
