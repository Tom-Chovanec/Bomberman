#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "level.hpp"
#include <memory>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "player.hpp"
#include "texture_manager.hpp"

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
    std::unique_ptr<TextureManager> tm;
    std::unique_ptr<LevelManager> lm;
    Uint64 prevTicks;
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

    if (!SDL_CreateWindowAndRenderer("Bomberman", 800, 600, SDL_WINDOW_RESIZABLE, &win, &ren)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    as->window.reset(win);
    as->renderer.reset(ren);
    as->player = std::make_unique<Player>();
    as->tm = std::make_unique<TextureManager>();
    as->lm = std::make_unique<LevelManager>();
    as->prevTicks = SDL_GetTicks();

    as->tm->load(as->renderer.get(), "player", "player.png");
    as->tm->load(as->renderer.get(), "ground", "ground.png");
    as->tm->load(as->renderer.get(), "wall", "wall.png");

    Level testLevel = {
        .width = 16,
        .height = 12,
        .layout = "XXXXXXXXXXXXXXXXX00000000000000XX00000000000000XX00000000000000XX00000000000000XX00000000000000XX00000000000000XX00000000000000XX00000000000000XX00000000000000XX00000000000000XXXXXXXXXXXXXXXXX"
    };
    as->lm->addLevel("text", testLevel);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    auto* as = static_cast<AppState*>(appstate);
    switch (event->type) {
        case (SDL_EVENT_QUIT): {
            return SDL_APP_SUCCESS;
        }
    }
    as->player->handleEvent(event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState* as = static_cast<AppState*>(appstate);
    Uint64 ticks = SDL_GetTicks();
    double dt = (ticks - as->prevTicks) / 1000.0;
    as->prevTicks = ticks;

    as->player->update(dt);

    SDL_SetRenderScale(as->renderer.get(), 2, 2);

    SDL_SetRenderDrawColor(as->renderer.get(), 18, 18, 18, 255);
    SDL_RenderClear(as->renderer.get());
    as->lm->renderLevel(as->renderer.get(), as->tm.get(), "text");

    auto playerRect = as->player->getRect();
    SDL_RenderTexture(as->renderer.get(), as->tm->get("player"), nullptr, &playerRect);

    SDL_RenderPresent(as->renderer.get());

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    delete static_cast<AppState*>(appstate);
}

