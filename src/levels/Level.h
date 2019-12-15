#pragma once

#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../characters/Player.h"
#include "../characters/Enemy.h"
#include "Levels.h"

#define ENCRYPTION_TYPE_RLE_ROW 0
#define ENCRYPTION_TYPE_RLE_COL 1
#define ENCRYPTION_TYPE_GRID 2

class Level {

  public: 

    Level() {};
        
    LevelElement getLevelData(const uint8_t x, const uint8_t y);
    void loadLevel(Player *player, Enemy enemies[]);                // Load level into memory.
    bool pickupGold();                                              // Returns true if last gold piece has been picked up.

    uint8_t getHeight();
    uint8_t getWidth();
    int16_t getXOffset();
    int16_t getYOffset();
    int8_t getXOffsetDelta();
    int8_t getYOffsetDelta();
    uint8_t getLevelNumber();
    uint8_t getGoldLeft();
    uint8_t getLevelLadderElementCount();
    LevelPoint getLevelLadderElement(const uint8_t index);
    LevelPoint getNextReentryPoint();
    
    void setLevelData(const uint8_t x, const uint8_t y, const LevelElement levelElement);
    void setXOffset(int16_t val);
    void setYOffset(int16_t val);
    void setXOffsetDelta(int8_t val);
    void setYOffsetDelta(int8_t val);
    void setLevelNumber(uint8_t val);
    void setGoldLeft(uint8_t val);

  private:

    static const uint8_t width = 14;
    static const uint8_t height = 16;

    int16_t xOffset;
    int16_t yOffset;
    int8_t xOffsetDelta;
    int8_t yOffsetDelta;
    uint8_t levelData[width][height];
    uint8_t levelNumber;
    uint8_t goldLeft;

    uint8_t levelLadderElementCount;
    uint8_t reentryPointIndex;

    LevelPoint levelLadder[18];
    LevelPoint reentryPoint[4];

};
