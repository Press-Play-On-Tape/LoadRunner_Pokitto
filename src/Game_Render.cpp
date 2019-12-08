#include "Game.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

#include "utils/Utils.h"
#include "images/Images.h"


// ------------------------------------------------------------------------------------------
//  Render the screen, players and scoreboard ..
// ------------------------------------------------------------------------------------------

void Game::renderScreen() {

  if (isFrameCount(12)) this->flashPlayer = !this->flashPlayer;

  if (gameState != GameState::NextLevel && gameState != GameState::GameOver && gameState != GameState::RestartLevel) {

    renderLevelElements();


    // Draw player ..

    if (gameState == GameState::LevelPlay || flashPlayer) {

      boolean flip = (static_cast<int8_t>(player.getStance()) < 0);
      //arduboy.drawCompressedMirror(player.getX(), player.getY(), men[absT(static_cast<int8_t>(player.getStance()))], WHITE, flip);
      PD::drawBitmap(player.getX(), player.getY(), Images::Men[absT(static_cast<int8_t>(player.getStance()))], 0, flip);

    }

    renderEnemies();
    renderArrows();

  }


  // Draw entry rectangle ..

  renderEntryRectangle();


  // Draw footer ..

//  arduboy.fillRect(0, 55, 128, 64, BLACK);
//SJH Below
  PD::setColor(0);
  PD::fillRect(0, 55, 220, 64 - 55);
  PD::setColor(1);
//  arduboy.drawHorizontalDottedLine(0, 220, 56);
  drawDottedColumn(0, 220, 56);


  // Draw scoreboard ..

  renderScoreboard();

}


// ------------------------------------------------------------------------------------------
//  Render the level elements (bricks, ladders, rails, etc) ..
// ------------------------------------------------------------------------------------------

void Game::renderLevelElements() {

  for (uint8_t y = 0; y < level.getHeight(); y++) {

    for (uint8_t x = 0; x < level.getWidth() * 2; x++) {

      int16_t tx = level.getXOffset() + (x * GRID_SIZE);
      // printf("%i ",tx);
      int16_t ty = level.getYOffset() + (y * GRID_SIZE);

      if (tx > -GRID_SIZE && tx < 220 && ty > -GRID_SIZE && ty < 64) {


        LevelElement element = (LevelElement)level.getLevelData(x, y);

        if (static_cast<uint8_t>(element) > 0) 
//          Sprites::drawOverwrite(tx, ty, levelElementImgs, static_cast<uint8_t>(element));
          PD::drawBitmap(tx, ty, Images::levelElementImgs[static_cast<uint8_t>(element)]);

        if (element >= LevelElement::Brick_1 && element <= LevelElement::Brick_4) {

//          Sprites::drawOverwrite(tx, ty - GRID_SIZE, brickDiggingImgs, static_cast<uint8_t>(element) - static_cast<uint8_t>(LevelElement::Brick_1));
          PD::drawBitmap(tx, ty - GRID_SIZE, Images::brickDiggingImgs[static_cast<uint8_t>(element) - static_cast<uint8_t>(LevelElement::Brick_1)]);

        }

      }
      // printf("\n");

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Render the enemies ..
// ------------------------------------------------------------------------------------------

void Game::renderEnemies() {

  for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

    Enemy *enemy = &enemies[x];

    if (enemy->getEnabled()) {

      const auto ex = enemy->getX() + level.getXOffset();
      const auto ey = enemy->getY() + level.getYOffset();

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
          image = Images::Man1; //SJH man_LaddderRight;
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

//        arduboy.drawCompressedMirror(dx, dy, image, WHITE, flip);
        PD::drawBitmap(dx, dy, image, false, flip);

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Render arrows that point to the enemies ..
// ------------------------------------------------------------------------------------------

#ifdef TWO_ARROW_SIZES

// ------------------------------------------------------------------------------------------
//  Render the arrows that indicate enemy positions ..
//
void Game::renderArrows() {

  renderArrows(true);
  renderArrows(false);
  
}


// ------------------------------------------------------------------------------------------
//  Render the arrows that indicate enemy positions.  Render small arrows first followed by
//  the larger ones.
//
void Game::renderArrows(bool smallArrows) {

  int16_t px = player.getX() - level.getXOffset();
  int16_t py = player.getY() - level.getYOffset();

  for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

    Enemy *enemy = &enemies[x];

    if (enemy->getEnabled()) {

      int16_t dx = px - enemy->getX();
      int16_t dy = py - enemy->getY();

      uint16_t absDX = (dx > 0 ? absT(-level.getXOffset() - enemy->getX()) : absT(enemy->getX() + level.getXOffset() - 220));
      uint16_t absDY = (dy > 0 ? absT(-level.getYOffset() - enemy->getY()) : absT(enemy->getY() + level.getYOffset() - HEIGHT_LESS_TOOLBAR));

      uint8_t picX = 255;
      uint8_t picY = 155;

      uint8_t const * mask = arrow_TL_mask;
      uint8_t const * image = arrow_TL;
      
      if (player.getY() - dy < -9) {

        if (player.getX() - dx < -1) {

          if (smallArrows && absDX > ARROW_SMALL_DIST_X_LEFT && absDY > ARROW_SMALL_DIST_Y_TOP) {

            picX = 0;
            picY = 0;
            
            image = arrow_TL_Sml;
            mask = arrow_TL_Sml_mask;
            
          }
          else if (!smallArrows && absDX <= ARROW_SMALL_DIST_X_LEFT && absDY <= ARROW_SMALL_DIST_Y_TOP) {

            picX = 0;
            picY = 0;

            image = arrow_TL;
            mask = arrow_TL_mask;

          }
          
        }
        else if (player.getX() - dx > 123) {

          if (smallArrows && absDX > ARROW_SMALL_DIST_X_RIGHT && absDY > ARROW_SMALL_DIST_Y_TOP) {

            picX = 123;
            picY = 0;

            image = arrow_TR_Sml;
            mask = arrow_TR_Sml_mask;

          }
          else if (!smallArrows && absDX <= ARROW_SMALL_DIST_X_RIGHT && absDY <= ARROW_SMALL_DIST_Y_TOP) {

            picX = 123;
            picY = 0;

            image = arrow_TR;
            mask = arrow_TR_mask;

          }

        }
        else {

          if (smallArrows && absDY > ARROW_SMALL_DIST_Y_TOP) {

            picX = player.getX() - dx + 1;
            picY = 0;

            image = arrow_TM_Sml;
            mask = arrow_TM_Sml_mask;

          }
          else if (!smallArrows && absDY < ARROW_SMALL_DIST_Y_TOP) {

            picX = player.getX() - dx + 1;
            picY = 0;

            image = arrow_TM;
            mask = arrow_TM_mask;

          }

        }

      }

      else if (player.getY() - dy > 52) { 

        if (player.getX() - dx < -1) {

          if (smallArrows && absDX > ARROW_SMALL_DIST_X_LEFT && absDY > ARROW_SMALL_DIST_Y_BOTTOM) {

            picX = 0;
            picY = 50;

            image = arrow_BL_Sml;
            mask = arrow_BL_Sml_mask;

          }
          else if (!smallArrows && absDX <= ARROW_SMALL_DIST_X_LEFT && absDY <= ARROW_SMALL_DIST_Y_BOTTOM) {

            picX = 0;
            picY = 50;

            image = arrow_BL;
            mask = arrow_BL_mask;

          }
          
        }
        else if (player.getX() - dx > 123) {

          if (smallArrows && absDX > ARROW_SMALL_DIST_X_RIGHT && absDY > ARROW_SMALL_DIST_Y_BOTTOM) {

            picX = 123;
            picY = 50;

            image = arrow_BR_Sml;
            mask = arrow_BR_Sml_mask;

          }
          else if (!smallArrows && absDX <= ARROW_SMALL_DIST_X_RIGHT && absDY <= ARROW_SMALL_DIST_Y_BOTTOM) {

            picX = 123;
            picY = 50;

            image = arrow_BR;
            mask = arrow_BR_mask;

          }

        }
        else {

          if (smallArrows && absDY > ARROW_SMALL_DIST_Y_BOTTOM) {

            picX = player.getX() - dx + 1;
            picY = 51;

            image = arrow_BM_Sml;
            mask = arrow_BM_Sml_mask;

          }
          else if (!smallArrows && absDY <= ARROW_SMALL_DIST_Y_BOTTOM) {

            picX = player.getX() - dx + 1;
            picY = 51;

            image = arrow_BM;
            mask = arrow_BM_mask;

          }

        }

      }
      else {

        if (player.getX() - dx < -4) {

          if (smallArrows && absDX > ARROW_SMALL_DIST_X_LEFT) {

            picX = 0;
            picY = player.getY() - dy;

            image = arrow_ML_Sml;
            mask = arrow_ML_Sml_mask;

          }
          else if (!smallArrows && absDX <= ARROW_SMALL_DIST_X_LEFT) {

            picX = 0;
            picY = player.getY() - dy;

            image = arrow_ML;
            mask = arrow_ML_mask;

          }          
          
        }
        else if (player.getX() - dx > 123) {

          if (smallArrows && absDX > ARROW_SMALL_DIST_X_RIGHT) {

            picX = 124;
            picY = player.getY() - dy;

            image = arrow_MR_Sml;
            mask = arrow_MR_Sml_mask;

          }
          else if (!smallArrows && absDX <= ARROW_SMALL_DIST_X_RIGHT) {           

            picX = 124;
            picY = player.getY() - dy;

            image = arrow_MR;
            mask = arrow_MR_mask;

          }

        }

      }

      if (picX != 255) {

        arduboy.drawCompressedMirror(picX, picY, mask, BLACK, false);
        arduboy.drawCompressedMirror(picX, picY, image, WHITE, false);

      }

    }

  }

}

#endif

#ifndef TWO_ARROW_SIZES

void Game::renderArrows() {

  int16_t px = player.getX() - level.getXOffset();
  int16_t py = player.getY() - level.getYOffset();

  for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

    Enemy *enemy = &enemies[x];

    if (enemy->getEnabled()) {

      int16_t dx = px - enemy->getX();
      int16_t dy = py - enemy->getY();

      uint8_t picX = 255;
      uint8_t picY = 155;

      //uint8_t const * mask = Images::arrow_TL_mask;
      uint8_t const * image = Images::ArrowTL;
      
      if (player.getY() - dy < -9) {

        if (player.getX() - dx < -1) {

          picX = 0;
          picY = 0;
          
          image = Images::ArrowTL_Small;
          // mask = arrow_TL_Sml_mask;
          
        }
        else if (player.getX() - dx > 123) {

          picX = 123;
          picY = 0;

          image = Images::ArrowTR_Small;
          // mask = Images::arrow_TR_Sml_mask;

        }
        else {

          picX = player.getX() - dx + 1;
          picY = 0;

          image = Images::ArrowTM_Small;
          // mask = Images::arrow_TM_Sml_mask;

        }

      }

      else if (player.getY() - dy > 52) { 

        if (player.getX() - dx < -1) {

          picX = 0;
          picY = 50;

          image = Images::ArrowBL_Small;
          // mask = Images::arrow_BL_Sml_mask;
          
        }
        else if (player.getX() - dx > 123) {

          picX = 123;
          picY = 50;

          image = Images::ArrowBR_Small;
          // mask = Images::arrow_BR_Sml_mask;

        }
        else {

          picX = player.getX() - dx + 1;
          picY = 51;

          image = Images::ArrowBM_Small;
          // mask = Images::arrow_BM_Sml_mask;

        }

      }
      else {

        if (player.getX() - dx < -4) {

          picX = 0;
          picY = player.getY() - dy;

          image = Images::ArrowML_Small;
          // mask = arrow_ML_Sml_mask;
          
        }
        else if (player.getX() - dx > 123) {

          picX = 124;
          picY = player.getY() - dy;

          image = Images::ArrowMR_Small;
          // mask = arrow_MR_Sml_mask;

        }

      }

      if (picX != 255) {

        // arduboy.drawCompressedMirror(picX, picY, mask, BLACK, false);
        // arduboy.drawCompressedMirror(picX, picY, image, WHITE, false);
        PD::drawBitmap(picX, picY, image, false, false);

      }

    }

  }

}

#endif


// ------------------------------------------------------------------------------------------
//  Render the entry / exit rectangle ..
// ------------------------------------------------------------------------------------------

void Game::renderEntryRectangle() {

  if (gameState == GameState::LevelEntryAnimation || gameState == GameState::LevelExitAnimation) {

    // arduboy.drawRect(introRect, introRect, 220 - (introRect * 2), 55 - (introRect * 2), BLACK);
    // arduboy.drawHorizontalDottedLine(0, 220, introRect);
    // arduboy.drawHorizontalDottedLine(0, 220, 54 - introRect);
    // arduboy.drawVerticalDottedLine(0, 64, introRect);
    // arduboy.drawVerticalDottedLine(0, 64, 127 - introRect);

    PD::setColor(0);
    PD::drawRect(introRect, introRect, 220 - (introRect * 2), 55 - (introRect * 2));
    PD::setColor(1);

    drawDottedRow(0, 220, introRect);
    drawDottedRow(0, 220, 54 - introRect);    
    drawDottedColumn(0, 64, introRect);
    drawDottedColumn(0, 64, 127 - introRect);

    if( gameState == GameState::LevelEntryAnimation) {

      for (int8_t x = introRect - 1; x >= 0; x--) {

//        arduboy.drawRect(x, x, 127 - (x * 2) + 1, 54 - (x * 2) + 1, BLACK);
        PD::setColor(0);
        PD::drawRect(x, x, 127 - (x * 2) + 1, 54 - (x * 2) + 1);
        PD::setColor(1);

      }
      introRect--;

      if (introRect == -1) gameState = GameState::LevelFlash;

    }
    else {

      for (int8_t x = 0; x < introRect; x++) {

//        arduboy.drawRect(x, x, 127 - (x * 2) + 1, 54 - (x * 2) + 1, BLACK);
        PD::setColor(0);
        PD::drawRect(x, x, 127 - (x * 2) + 1, 54 - (x * 2) + 1);
        PD::setColor(1);

      }
      introRect++;

      // Game over, restart level or next level ?
      if (introRect == LEVEL_ANIMATION_BANNER_WIDTH) gameState = player.getNextState();

    }

  }
  else if (gameState == GameState::NextLevel || gameState == GameState::RestartLevel || gameState == GameState::GameOver) {

    if(gameState == GameState::RestartLevel) {

//      arduboy.drawCompressedMirror(42, 25, tryAgain, WHITE, false);
      PD::drawBitmap(42, 25, Images::TryAgain);

    }
    else if(gameState == GameState::GameOver) {

//      arduboy.drawCompressedMirror(43, 25, gameOver, WHITE, false);
      PD::drawBitmap(42, 25, Images::GameOver);

    }
    else {

      uint8_t levelNumber = level.getLevelNumber();
//      Sprites::drawOverwrite(72, 25, numbers, levelNumber / 100);
      PD::drawBitmap(72, 25, Images::Numbers[levelNumber / 100]);
      levelNumber = levelNumber - (levelNumber / 100) * 100;
      
    //   Sprites::drawOverwrite(77, 25, numbers, levelNumber / 10);
    //   Sprites::drawOverwrite(82, 25, numbers, levelNumber % 10);
      PD::drawBitmap(77, 25, Images::Numbers[levelNumber / 10]);
      PD::drawBitmap(82, 25, Images::Numbers[levelNumber % 10]);

//      arduboy.drawCompressedMirror(43, 25, levelChange, WHITE, false);
      PD::drawBitmap(43, 25, Images::LevelChange);

    }

    // arduboy.drawHorizontalDottedLine(41, 87, 22);
    // arduboy.drawHorizontalDottedLine(41, 87, 32);
    drawDottedRow(41, 87, 22);
    drawDottedRow(41, 87, 32);
  
  }

}


// ------------------------------------------------------------------------------------------
//  Render the scorebaord ..
//
void Game::renderScoreboard() {


  // Score ..
  {
      uint16_t score = player.getScore();
      uint8_t digits[6] = {};
      extractDigits(digits, score);
      
//      arduboy.drawCompressedMirror(0, 58, score_sc, WHITE, false);
      PD::drawBitmap(0, 58, Images::Score);
      for(uint8_t i = 0, x = 54; i < 6; ++i, x -= 5) {
        PD::drawBitmap(x, 58, Images::Numbers[digits[i]]);
      }
  }


  // Men left ..

  uint8_t menLeft = player.getMen();
//   arduboy.drawCompressedMirror(64, 58, men_sc, WHITE, false);
//   Sprites::drawOverwrite(82, 58, numbers, menLeft / 10);
//   Sprites::drawOverwrite(87, 58, numbers, menLeft % 10);
  PD::drawBitmap(64, 58, Images::Men_SC);
  PD::drawBitmap(82, 58, Images::Numbers[menLeft / 10]);
  PD::drawBitmap(87, 58, Images::Numbers[menLeft % 10]);


  // Gold or level ..
  
  if (gameState == GameState::LevelPlay) {

    uint8_t goldLeft = level.getGoldLeft();
    // arduboy.drawCompressedMirror(96, 58, gold_sc, WHITE, false);
    // Sprites::drawOverwrite(118, 58, numbers, goldLeft / 10);
    // Sprites::drawOverwrite(123, 58, numbers, goldLeft % 10);
    PD::drawBitmap(96, 58, Images::Gold_SC);
    PD::drawBitmap(118, 58, Images::Numbers[goldLeft / 10]);
    PD::drawBitmap(123, 58, Images::Numbers[ goldLeft % 10]);

  }
  else {

    uint8_t levelNumber = level.getLevelNumber();
//    arduboy.drawCompressedMirror(96, 58, level_sc, WHITE, false);
    PD::drawBitmap(96, 58, Images::Level);
    
    const auto divT = div(levelNumber, 100);
//    Sprites::drawOverwrite(113, 58, numbers, divT.quot);
    PD::drawBitmap(113, 58, Images::Numbers[divT.quot]);
    levelNumber = divT.rem;
    PD::drawBitmap(118, 58, Images::Numbers[levelNumber / 10]);
    PD::drawBitmap(123, 58, Images::Numbers[levelNumber % 10]);

  }

}
