#include <memory>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "player.hpp"

struct SDL_Window_Deleter {
    void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
};
struct SDL_Renderer_Deleter {
    void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
};

struct AppState {
    std::unique_ptr<SDL_Window, SDL_Window_Deleter> window;
    std::unique_ptr<SDL_Renderer, SDL_Renderer_Deleter> renderer;
    std::unique_ptr<Player> player;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    AppState *as = new AppState();
    if (!as) {
        return SDL_APP_FAILURE;
    }
    *appstate = as;

    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;

    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_RESIZABLE, &win, &ren)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    as->window.reset(win);
    as->renderer.reset(ren);
    as->player = std::make_unique<Player>();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    auto* as = static_cast<AppState*>(appstate);
    switch (event->type) {
        case (SDL_EVENT_QUIT): {
            return SDL_APP_SUCCESS;
        }
        case (SDL_EVENT_KEY_DOWN): {
            switch (event->key.key){
                case SDLK_W:
                    SDL_LogInfo(0, "%i", as->player->getHealth());
            }
        }
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState* as = static_cast<AppState*>(appstate);
    const char *message = "Hello World!";
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;

    SDL_GetRenderOutputSize(as->renderer.get(), &w, &h);
    SDL_SetRenderScale(as->renderer.get(), scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    SDL_SetRenderDrawColor(as->renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(as->renderer.get());
    SDL_SetRenderDrawColor(as->renderer.get(), 255, 255, 255, 255);
    SDL_RenderDebugText(as->renderer.get(), x, y, message);
    SDL_RenderPresent(as->renderer.get());

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    delete static_cast<AppState*>(appstate);
}

