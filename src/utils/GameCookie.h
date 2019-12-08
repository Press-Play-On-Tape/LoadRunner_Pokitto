#pragma once

#include "Pokitto.h"
#include "PokittoCookie.h"
#include "Enums.h"

class GameCookie : public Pokitto::Cookie {

  public:
    uint8_t  gameNumber = 1;
    uint8_t  levelNumber = 1;
    uint8_t  menLeft = 5;
    uint16_t score = 0;
    uint8_t  gameNumberOrig = 1;
    uint8_t  levelNumberOrig = 1;
    uint8_t  menLeftOrig = 5;
    uint16_t scoreOrig = 0;

};