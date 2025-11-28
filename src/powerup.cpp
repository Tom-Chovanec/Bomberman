#include "powerup.hpp"


Powerup::Powerup(PowerupType type) 
    : type(type) {

}

PowerupType Powerup::getType() const {
    return type;
}
