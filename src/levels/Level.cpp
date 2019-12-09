#include "Level.h"


//--------------------------------------------------------------------------------------------------------------------------

uint8_t Level::getWidth() {
    return this->width;
}

uint8_t Level::getHeight() {
    return this->height;
}

int16_t Level::getXOffset() {
    return this->xOffset;
}

int16_t Level::getYOffset() {
    return this->yOffset;
}

int8_t Level::getXOffsetDelta() {
    return this->xOffsetDelta;
}

int8_t Level::getYOffsetDelta() {
    return this->yOffsetDelta;
}

uint8_t Level::getLevelNumber() {
    return this->levelNumber;
}

uint8_t Level::getGoldLeft() {
    return this->goldLeft;
}

uint8_t Level::getLevelLadderElementCount() {
    return this->levelLadderElementCount;
}

void Level::setXOffset(int16_t val) {
    this->xOffset = val;
}

void Level::setYOffset(int16_t val) {
    this->yOffset = val;
}

void Level::setXOffsetDelta(int8_t val) {
    this->xOffsetDelta = val;
}

void Level::setYOffsetDelta(int8_t val) {
    this->yOffsetDelta = val;
}

void Level::setLevelNumber(uint8_t val) {
    this->levelNumber = val;
}

void Level::setGoldLeft(uint8_t val) {
    this->goldLeft = val;
}

LevelPoint Level::getLevelLadderElement(const uint8_t index) {
  return this->levelLadder[index];
}

LevelPoint Level::getNextReentryPoint() {
  return this->reentryPoint[this->reentryPointIndex];
  this->reentryPointIndex = (this->reentryPointIndex == 3 ? 0 : this->reentryPointIndex + 1);
}


// -----------------------------------------------------------------------------------------------
//  Load level data ..
//
void Level::loadLevel(Player *player, Enemy enemies[]) {

  uint16_t dataOffset = 0;
  uint8_t goldLeft = 0;

  const uint8_t *levelToLoad = levels[this->levelNumber];
  player->setStance(PlayerStance::Running_Right1);


  // Load player starting position ..

//   uint16_t  playerX = pgm_read_byte(&levelToLoad[dataOffset++]) * GRID_SIZE;
//   uint16_t  playerY = pgm_read_byte(&levelToLoad[dataOffset++]) * GRID_SIZE;
  uint16_t playerX = levelToLoad[dataOffset++] * GRID_SIZE;
  uint16_t playerY = levelToLoad[dataOffset++] * GRID_SIZE;


  // Determine player's X Pos and level offset ..

  if (playerX < (220 / 2) - 5) {

    this->xOffset = 0;
    player->setX(playerX);

  }
  else {

    if (playerX >= (220 / 2) - 5 && playerX <= (this->width * GRID_SIZE * 2) - 220) {

      player->setX((220 / 2) - 5);
      this->xOffset = player->getX() - playerX;


    }
    else {

      this->xOffset = -2;
      player->setX(playerX + this->xOffset);

    }

  }


  // Determine player's Y Pos and level offset ..

  if (playerY < (HEIGHT_LESS_TOOLBAR / 2) - 5) {
    this->yOffset = 0;
    player->setY(playerY);

  }
  else {

    if (playerY >= (HEIGHT_LESS_TOOLBAR / 2) - 5 && playerY <= (this->height * GRID_SIZE) - HEIGHT_LESS_TOOLBAR) {

      player->setY((HEIGHT_LESS_TOOLBAR / 2) - 5);
      this->yOffset = player->getY() - playerY;

    }
    else {

      this->yOffset = -2;
      player->setY(playerY + this->yOffset);

    }

  }


  // Load enemies ..

  uint8_t numberOfEnemies = levelToLoad[dataOffset++];

  for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

    Enemy *enemy = &enemies[x];

    enemy->setId(x);
    enemy->setGoldCountdown(0);

    if (x < numberOfEnemies) {

      enemy->setX(levelToLoad[dataOffset++] * GRID_SIZE);
      enemy->setY(levelToLoad[dataOffset++] * GRID_SIZE);
      enemy->setEnabled(true);

    }
    else {

      enemy->setEnabled(false);

    }

  }


  // Load level ladder points ..

//  _levelLadderElementCount = pgm_read_byte(&levelToLoad[dataOffset++]);
  this->levelLadderElementCount = levelToLoad[dataOffset++];

  for (uint8_t x = 0; x < this->levelLadderElementCount; x++) {

    // _levelLadder[x].x = pgm_read_byte(&levelToLoad[dataOffset++]);
    // _levelLadder[x].y = pgm_read_byte(&levelToLoad[dataOffset++]);
    this->levelLadder[x].x = levelToLoad[dataOffset++];
    this->levelLadder[x].y = levelToLoad[dataOffset++];

  }


  // Load reentry points ..

  for (uint8_t x = 0; x < NUMBER_OF_REENTRY_POINTS; x++) {

    // _reentryPoint[x].x = pgm_read_byte(&levelToLoad[dataOffset++]);
    // _reentryPoint[x].y = pgm_read_byte(&levelToLoad[dataOffset++]);
    this->reentryPoint[x].x = levelToLoad[dataOffset++];
    this->reentryPoint[x].y = levelToLoad[dataOffset++];

  }


  // Load level data .. 

  uint8_t encryptionType = levelToLoad[dataOffset++];
  
  if (encryptionType == ENCRYPTION_TYPE_GRID) {

    for (uint8_t y = 0; y < this->height; y++) {

      for (uint8_t x = 0; x < this->width; x++) {

        uint8_t data = levelToLoad[(y * this->width) + x + dataOffset];

        if (Utils::leftValue(data) == static_cast<uint8_t>(LevelElement::Gold))            { goldLeft++;}
        if (Utils::rightValue(data) == static_cast<uint8_t>(LevelElement::Gold))           { goldLeft++;}

        this->levelData[x][y] = data;

      }

    }

  }
  else {
	  
    uint16_t cursor = 0;

    while (true) {

      uint8_t data = levelToLoad[dataOffset];
      uint8_t block = (data & 0xE0) >> 5;
      uint8_t run = data & 0x1F;

      if (block == static_cast<uint8_t>(LevelElement::Gold))            { goldLeft = goldLeft + run;}

      if (run > 0) {

        dataOffset++;

        for (uint8_t x = 0; x < run; x++) {

          if (encryptionType == ENCRYPTION_TYPE_RLE_ROW) {
            
            uint8_t row = cursor / (this->width * 2);
            uint8_t col = (cursor % (this->width * 2)) / 2;

            if (cursor % 2 == 0) {
              this->levelData[col][row] = (this->levelData[col][row] & 0x0f) | (block << 4);
            } 
            else {
              this->levelData[col][row] = (this->levelData[col][row] & 0xF0) | block;
            }

          }
          else {

            uint8_t col = cursor / this->height;
            uint8_t row = cursor % this->height;

            if (col % 2 == 0) {
              this->levelData[col / 2][row] = (this->levelData[col / 2][row] & 0x0f) | (block << 4);
            } 
            else {
              this->levelData[col / 2][row] = (this->levelData[col / 2][row] & 0xF0) | block;
            }

          }

          cursor++;

        }

      }
      else {
      
        break;
      
      }

    }

  }

  this->goldLeft = goldLeft;

}


// -----------------------------------------------------------------------------------------------
//  Get level element at position x and y ..
//
LevelElement Level::getLevelData(const uint8_t x, const uint8_t y) {

  if ((x / 2) >= this->width) return LevelElement::Brick;
  if (y == 255) return LevelElement::Blank;
  if (y >= this->height) return LevelElement::Solid;

  if (x % 2 == 0) {

    return static_cast<LevelElement>(this->levelData[x / 2][y] >> 4); 

  }
  else {

    return static_cast<LevelElement>(this->levelData[x / 2][y] & 0x0F); 

  }

  return LevelElement::Brick;
      
}


// -----------------------------------------------------------------------------------------------
//  Set level element at position x and y ..
//
void Level::setLevelData(const uint8_t x, const uint8_t y, const LevelElement levelElement) {

  if (x % 2 == 0) {

    this->levelData[x / 2][y] = (this->levelData[x / 2][y] & 0x0f) | (static_cast<uint8_t>(levelElement) << 4); 

  }
  else {

    this->levelData[x / 2][y] = (this->levelData[x / 2][y] & 0xf0) | static_cast<uint8_t>(levelElement); 
    
  }
      
}


// -----------------------------------------------------------------------------------------------
//  Update the level when the last gold is collected ..
//
void Level::pickupGold() {

  if (this->goldLeft > 0) this->goldLeft--;

  if (this->goldLeft == 0) {

    // Update map with level ladder ..

    for (uint8_t x = 0; x < this->levelLadderElementCount; x++) {

      LevelPoint lp = this->levelLadder[x];

      Level::setLevelData(lp.x, lp.y, LevelElement::Ladder);

    }

  }

}