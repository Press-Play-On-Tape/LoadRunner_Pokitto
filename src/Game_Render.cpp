#include "Game.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

#include "utils/Utils.h"
#include "images/Images.h"


// ------------------------------------------------------------------------------------------
//  Render the screen, players and scoreboard ..
// ------------------------------------------------------------------------------------------

void Game::renderScreen() {

  if (Utils::isFrameCount(12)) this->flashPlayer = !this->flashPlayer;

  if (this->gameState != GameState::NextLevel && this->gameState != GameState::GameOver && this->gameState != GameState::RestartLevel) {

    renderLevelElements();


    // Draw player ..

    if (gameState == GameState::LevelPlay || flashPlayer) {

      boolean flip = (static_cast<int8_t>(this->player.getStance()) < 0);
      PD::drawBitmap(this->player.getX(), this->player.getY(), Images::Men[absT(static_cast<int8_t>(this->player.getStance()))], 0, flip);

    }

    this->renderEnemies();

  }


  // Draw entry rectangle ..

  this->renderEntryRectangle();


  // Draw footer ..

  PD::setColor(0);
  PD::fillRect(0, 169, 220, 9);
  PD::setColor(1);
  Utils::drawDottedColumn(0, 220, 170);


  // Draw scoreboard ..

  this->renderScoreboard();

}


// ------------------------------------------------------------------------------------------
//  Render the level elements (bricks, ladders, rails, etc) ..
// ------------------------------------------------------------------------------------------

void Game::renderLevelElements() {

  for (uint8_t y = 0; y < this->level.getHeight(); y++) {

    for (uint8_t x = 0; x < this->level.getWidth() * 2; x++) {

      int16_t tx = this->level.getXOffset() + (x * GRID_SIZE);
      int16_t ty = this->level.getYOffset() + (y * GRID_SIZE);

      if (tx > -GRID_SIZE && tx < 220 && ty > -GRID_SIZE && ty < 176) {


        LevelElement element = (LevelElement)this->level.getLevelData(x, y);

        if (static_cast<uint8_t>(element) > 0) PD::drawBitmap(tx, ty, Images::levelElementImgs[static_cast<uint8_t>(element)]);

        if (element >= LevelElement::Brick_1 && element <= LevelElement::Brick_4) {

          PD::drawBitmap(tx, ty - GRID_SIZE, Images::brickDiggingImgs[static_cast<uint8_t>(element) - static_cast<uint8_t>(LevelElement::Brick_1)]);

        }

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Render the enemies ..
// ------------------------------------------------------------------------------------------

void Game::renderEnemies() {

  for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

    Enemy *enemy = &this->enemies[x];

    if (enemy->getEnabled()) {

      const auto ex = enemy->getX() + this->level.getXOffset();
      const auto ey = enemy->getY() + this->level.getYOffset();

      auto dx = ex;
      auto dy = ey;
      uint8_t const * image = Images::Man0;
      bool flip = false;

      switch (enemy->getEscapeHole()) {

        case EscapeHole::None:
          flip = (static_cast<int8_t>(enemy->getStance()) < 0);
          image = Images::Men[absT(static_cast<int8_t>(enemy->getStance()))];
          break;

        case EscapeHole::Wait1 ... EscapeHole::WaitMax:
          break;

        case EscapeHole::Wiggle1:
        case EscapeHole::Wiggle2:
        case EscapeHole::Wiggle5:
        case EscapeHole::Wiggle6:
          dx = ex - 1;
          break;

        case EscapeHole::Wiggle3:
        case EscapeHole::Wiggle4:
        case EscapeHole::Wiggle7:
        case EscapeHole::Wiggle8:
          dx = ex + 1;
          break;

        case EscapeHole::MoveUp9 ... EscapeHole::MoveUp10:
          dy = ey - 2;
          image = Images::Man0; //SJH man_LaddderLeft;
          break;

        case EscapeHole::MoveUp7 ... EscapeHole::MoveUp8:
          dy = ey - 4;
          image = Images::Man1; //SJH man_LaddderRight;
          break;

        case EscapeHole::MoveUp5 ... EscapeHole::MoveUp6:
          dy = ey - 6;
          image = Images::Man0; //SJH man_LaddderLeft;
          break;

        case EscapeHole::MoveUp3 ... EscapeHole::MoveUp4:
          dy = ey - 8;
          image = Images::Man1; //SJH man_LadderRight;
          break;

        case EscapeHole::MoveUp1 ... EscapeHole::MoveUp2:

          dy = ey - 10;
          image = Images::Man0; //SJH man_LaddderLeft;

          enemy->setY(enemy->getY() - 10);
          enemy->setEscapeHole(EscapeHole::None);
          setDirectionAfterHoleEscape(enemy);

          break;

        default:
          // Uncomment this if man_StandingStill being default is an issue
          // will increase code size by ~30 bytes
          //continue;
          break;

        }

        PD::drawBitmap(dx, dy, image, false, flip);

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Render the entry / exit rectangle ..
// ------------------------------------------------------------------------------------------

void Game::renderEntryRectangle() {

  if (gameState == GameState::LevelEntryAnimation || gameState == GameState::LevelExitAnimation) {

    PD::setColor(0);
    PD::drawRect(this->introRect, this->introRect, 220 - (this->introRect * 2), 168 - (this->introRect * 2));
    PD::setColor(1);

    Utils::drawDottedRow(0, 220, this->introRect);
    Utils::drawDottedRow(0, 220, 168 - this->introRect);    
    Utils::drawDottedColumn(0, 64, this->introRect);
    Utils::drawDottedColumn(0, 64, 219 - this->introRect);

    if (gameState == GameState::LevelEntryAnimation) {

      for (int8_t x = this->introRect - 1; x >= 0; x--) {

        PD::setColor(0);
        PD::drawRect(x, x, 220 - (x * 2) + 1, 168 - (x * 2) + 1);
        PD::setColor(1);

      }
      this->introRect--;

      if (this->introRect <= -1) gameState = GameState::LevelFlash;

    }
    else {

      for (int8_t x = 0; x < this->introRect; x++) {

        PD::setColor(0);
        PD::drawRect(x, x, 219 - (x * 2) + 1, 168 - (x * 2) + 1);
        PD::setColor(1);

      }
      this->introRect++;

      // Game over, restart level or next level ?
      if (this->introRect == LEVEL_ANIMATION_BANNER_WIDTH) gameState = this->player.getNextState();

    }

  }
  else if (this->gameState == GameState::NextLevel || this->gameState == GameState::RestartLevel || this->gameState == GameState::GameOver) {

    if (this->gameState == GameState::RestartLevel) {

      PD::drawBitmap(42, 25, Images::TryAgain);

    }
    else if(this->gameState == GameState::GameOver) {

      PD::drawBitmap(42, 25, Images::GameOver);

    }
    else {

      uint8_t levelNumber = this->level.getLevelNumber();
      PD::drawBitmap(72, 25, Images::Numbers[levelNumber / 100]);
      levelNumber = levelNumber - (levelNumber / 100) * 100;
      
      PD::drawBitmap(77, 25, Images::Numbers[levelNumber / 10]);
      PD::drawBitmap(82, 25, Images::Numbers[levelNumber % 10]);

      PD::drawBitmap(43, 25, Images::LevelChange);

    }

    Utils::drawDottedRow(41, 87, 22);
    Utils::drawDottedRow(41, 87, 32);
  
  }

}


// ------------------------------------------------------------------------------------------
//  Render the scorebaord ..
//
void Game::renderScoreboard() {


  // Score ..
  {
      
      uint16_t score = this->player.getScore();
      uint8_t digits[6] = {};
      Utils::extractDigits(digits, score);
      
      PD::drawBitmap(0, 170, Images::Score);
      for(uint8_t i = 0, x = 54; i < 6; ++i, x -= 5) {
        PD::drawBitmap(x, 170, Images::Numbers[digits[i]]);
      }
      
  }


  // Men left ..

  uint8_t menLeft = this->player.getMen();
  PD::drawBitmap(64, 170, Images::Men_SC);
  PD::drawBitmap(82, 170, Images::Numbers[menLeft / 10]);
  PD::drawBitmap(87, 170, Images::Numbers[menLeft % 10]);


  // Gold or level ..
  
  if (gameState == GameState::LevelPlay) {

    uint8_t goldLeft = this->level.getGoldLeft();
    PD::drawBitmap(96, 170, Images::Gold_SC);
    PD::drawBitmap(118, 170, Images::Numbers[goldLeft / 10]);
    PD::drawBitmap(123, 170, Images::Numbers[ goldLeft % 10]);

  }
  else {

    uint8_t levelNumber = this->level.getLevelNumber();
    PD::drawBitmap(96, 170, Images::Level);
    
    const auto divT = div(levelNumber, 100);
    PD::drawBitmap(113, 170, Images::Numbers[divT.quot]);
    levelNumber = divT.rem;
    PD::drawBitmap(118, 170, Images::Numbers[levelNumber / 10]);
    PD::drawBitmap(123, 170, Images::Numbers[levelNumber % 10]);

  }

}
