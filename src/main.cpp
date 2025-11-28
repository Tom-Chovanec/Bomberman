#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "level.hpp"
#include <array>
#include <chrono>
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
    std::array<std::unique_ptr<Player>, 2> players;
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
    as->players[0] = std::make_unique<Player>(
        0,
        std::array<SDL_Scancode, 5>{
            SDL_SCANCODE_W,
            SDL_SCANCODE_S,
            SDL_SCANCODE_A,
            SDL_SCANCODE_D,
            SDL_SCANCODE_E,
        }
    );
    as->players[1] = std::make_unique<Player>(
        1,
        std::array<SDL_Scancode, 5>{
            SDL_SCANCODE_UP,
            SDL_SCANCODE_DOWN,
            SDL_SCANCODE_LEFT,
            SDL_SCANCODE_RIGHT,
            SDL_SCANCODE_SLASH,
        }
    );
    as->tm = std::make_unique<TextureManager>();
    as->lm = std::make_unique<LevelManager>();
    as->prevTicks = SDL_GetTicks();

    as->tm->load(as->renderer.get(), "player", "player.png");
    as->tm->load(as->renderer.get(), "ground", "ground.png");
    as->tm->load(as->renderer.get(), "wall", "wall.png");
    as->tm->load(as->renderer.get(), "crate", "crate.png");
    as->tm->load(as->renderer.get(), "bomb", "bomb.png");
    as->tm->load(as->renderer.get(), "flame", "flame.png");
    as->tm->load(as->renderer.get(), "fire_up_upgrade", "fire_up_upgrade.png");
    as->tm->load(as->renderer.get(), "bomb_up_upgrade", "bomb_up_upgrade.png");
    as->tm->load(as->renderer.get(), "skate_upgrade", "skate_upgrade.png");

    as->lm->loadLevel("level1", "level1.l");

    return SDL_APP_CONTINUE;
}

void reset(void *appstate) {
    auto* as = static_cast<AppState*>(appstate);

    as->lm->reset();
    for (auto& player : as->players) {
        player->reset();
    }
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    auto* as = static_cast<AppState*>(appstate);
    switch (event->type) {
        case (SDL_EVENT_QUIT): {
            return SDL_APP_SUCCESS;
        }
        case (SDL_EVENT_KEY_DOWN): {
            switch (event->key.scancode) {
                case (SDL_SCANCODE_J): {
                    reset(as);
                    as->lm->setActiveLevel("level1");
                    SDL_FRect rect = as->lm->getActiveLevel().spawnZones[0];
                    as->players[0]->summon(SDL_FPoint{rect.x + 2, rect.y + 2});
                    rect = as->lm->getActiveLevel().spawnZones[1];
                    as->players[1]->summon(SDL_FPoint{rect.x + 2, rect.y + 2});
                    break;
                }
                case (SDL_SCANCODE_O):
                    as->lm->clearActiveLevel();
                    as->players[0]->setHealth(0);
                    as->players[1]->setHealth(0);
                    break;
                default:
                    break;
            }
            break;
        }
    }
    for (auto& player : as->players) {
        player->handleEvent(event);
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState* as = static_cast<AppState*>(appstate);
    Uint64 ticks = SDL_GetTicks();
    std::chrono::milliseconds dt(ticks - as->prevTicks);
    as->prevTicks = ticks;

    as->lm->update(dt, { as->players[0].get(), as->players[1].get() });

    for (auto& player : as->players) {
        player->update(dt, *as->lm);
    }

    SDL_SetRenderScale(as->renderer.get(), 2, 2);

    SDL_SetRenderDrawColor(as->renderer.get(), 18, 18, 18, 255);
    SDL_RenderClear(as->renderer.get());
    as->lm->renderLevel(as->renderer.get(), as->tm.get());

    for (auto& player : as->players) {
        if (player->isAlive()) {
            SDL_RenderTexture(as->renderer.get(), as->tm->get("player"), nullptr, &player->getSpriteRect());
        }
    }

    // if (as->player->isAlive()) {
    //     SDL_SetRenderDrawColor(as->renderer.get(), 255, 0, 255, 255);
    //     SDL_RenderFillRect(as->renderer.get(), &as->player->getRect());
    // }

    SDL_RenderPresent(as->renderer.get());

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    delete static_cast<AppState*>(appstate);
}

