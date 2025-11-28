#include "SDL3/SDL_init.h"
#include <array>
#include <chrono>
#include <memory>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include "player.hpp"
#include "texture_manager.hpp"
#include "level.hpp"
#include "ui.hpp"

constexpr float BUTTON_WIDTH = 200.0f;
constexpr float BUTTON_HEIGHT = 50.0f;
constexpr float SCREEN_WIDTH = 800.0f;
constexpr float SCREEN_HEIGHT = 600.0f;

enum class GameState {
    MENU,
    PLAYING
};

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
    std::unique_ptr<UI> ui;
    std::chrono::milliseconds prevTime;
    bool isGameOver = false;
    std::chrono::milliseconds gameOverTimeStamp = std::chrono::milliseconds(0);

    GameState state = GameState::MENU;
};

void startGame(AppState* as) {
    as->lm->reset();
    for (auto& player : as->players) {
        if(player) player->reset();
    }

    as->isGameOver = false;

    as->lm->setActiveLevel("level1");

    SDL_FRect rect = as->lm->getActiveLevel().spawnZones[0];
    as->players[0]->summon(SDL_FPoint{rect.x + 2, rect.y + 2});

    rect = as->lm->getActiveLevel().spawnZones[1];
    as->players[1]->summon(SDL_FPoint{rect.x + 2, rect.y + 2});

    as->state = GameState::PLAYING;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!TTF_Init()) {
        SDL_Log("Coildn't init TTF: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppState *as = new AppState();
    if (!as) {
        return SDL_APP_FAILURE;
    }
    *appstate = as;

    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;

    if (!SDL_CreateWindowAndRenderer("Bomberman", (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &win, &ren)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    as->window.reset(win);
    as->renderer.reset(ren);
    
    as->players[0] = std::make_unique<Player>(
        0,
        std::array<SDL_Scancode, 5>{ SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_E }
    );
    as->players[1] = std::make_unique<Player>(
        1,
        std::array<SDL_Scancode, 5>{ SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_SLASH }
    );
    as->tm = std::make_unique<TextureManager>();
    as->lm = std::make_unique<LevelManager>();
    as->ui = std::make_unique<UI>(as->renderer.get());
    as->prevTime = std::chrono::milliseconds(SDL_GetTicks());

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
    as->ui->setOnStartGame([as]{startGame(as);});

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
            if (as->state == GameState::MENU) {
                if (event->key.scancode == SDL_SCANCODE_RETURN || event->key.scancode == SDL_SCANCODE_KP_ENTER) {
                    startGame(as);
                }
            } else {
                switch (event->key.scancode) {
                    case (SDL_SCANCODE_ESCAPE):
                        as->state = GameState::MENU;
                        break;
                    default:
                        break;
                }
            }
            break;
        }
    }

    if (as->state == GameState::MENU) {
        as->ui->handleEvent(event);
    }

    if (as->state == GameState::PLAYING) {
        for (auto& player : as->players) {
            player->handleEvent(event);
        }
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState* as = static_cast<AppState*>(appstate);
    std::chrono::milliseconds time = std::chrono::milliseconds(SDL_GetTicks());
    std::chrono::milliseconds dt = time - as->prevTime;
    as->prevTime = time;

    SDL_SetRenderDrawColor(as->renderer.get(), 18, 18, 18, 255);
    SDL_RenderClear(as->renderer.get());

    if (as->state == GameState::MENU) {
        SDL_SetRenderScale(as->renderer.get(), 1, 1);

        as->ui->render(as->renderer.get());
    } 
    else if (as->state == GameState::PLAYING) {
        if (as->isGameOver) {
            if (time - as->gameOverTimeStamp >= std::chrono::seconds(1)) {
                as->state = GameState::MENU;
            }
        }
        as->lm->update(dt, { as->players[0].get(), as->players[1].get() });
        for (auto& player : as->players) {
            player->update(dt, *as->lm);
        }

        SDL_SetRenderScale(as->renderer.get(), 2, 2);
        as->lm->renderLevel(as->renderer.get(), as->tm.get());

        for (auto& player : as->players) {
            if (player->isAlive()) {
                SDL_SetTextureColorMod(as->tm->get("player"), 255 * player->getId(), 255, 255);
                SDL_RenderTexture(as->renderer.get(), as->tm->get("player"), nullptr, &player->getSpriteRect());
            } else if (!as->isGameOver){
                as->isGameOver = true;
                as->gameOverTimeStamp = time;
            }
        }
    }

    SDL_RenderPresent(as->renderer.get());
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    delete static_cast<AppState*>(appstate);
    TTF_Quit();
}
