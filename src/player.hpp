#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

class Player {
private:
    int health = 3;
    SDL_Point position = {0, 0};

public:
    ~Player();

    int getHealth();
    void setHealth(int health);

    SDL_Point getPosition();
    void setPosition(SDL_Point position);
};
