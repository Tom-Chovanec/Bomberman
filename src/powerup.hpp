#pragma once

enum class PowerupType {
    FIRE_UP = 0,
    BOMB_UP,
    SKATE,
};

class Powerup {
private:
    PowerupType type;

public:
    Powerup(PowerupType type);
    PowerupType getType() const;
};
