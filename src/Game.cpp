#include "Game.h"

#include "characters/Enemy.h"
#include "characters/Player.h"
#include "images/Images.h"
#include "utils/Utils.h"
#include "utils/EEPROM_Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


void Game::setup(GameCookie *cookie) { 
    
    this->cookie = cookie;
    
    EEPROM_Utils::getSavedGameData(*this->cookie, &level, &player);
    
    this->player.setX(20);
    this->player.setY(35);
    this->player.setStance(PlayerStance::StandingStill);
    
    this->player.setNextState(GameState::Intro);
    
    uint8_t gameNumber = EEPROM_Utils::getGameNumber(*this->cookie);
    	
}

void Game::loop(void) {
    
    PC::buttons.pollButtons();
    PD::clear();
    
    switch (gameState) {
        
        case GameState::Splash:
            //SJH if (!sound.playing()) sound.tones(score);
            splash();
            break;
        
        case GameState::Intro:
            //SJH if (!sound.playing()) sound.tones(score);
            intro();
            break;

        case GameState::LevelInit:
            this->levelShow = true;
            //SJH sound.noTone();
            while (!this->holes.isEmpty()) this->holes.dequeue();
            //      level.setLevelNumber(36);
            this->level.loadLevel(&player, enemies); 
            this->introRect = 77;
            this->gameState = GameState::LevelEntryAnimation;
            [[fallthrough]];
            
        case GameState::LevelEntryAnimation:
        case GameState::LevelFlash:
        case GameState::LevelPlay:
            levelPlay();
            break;
        
        case GameState::LevelExitInit:
            this->introRect = 0;
            this->gameState = GameState::LevelExitAnimation;
            [[fallthrough]];
        
        case GameState::LevelExitAnimation:
        case GameState::GameOver:
        case GameState::RestartLevel:
            levelPlay();
            break;
        
        case GameState::NextLevel:
            EEPROM_Utils::saveGameData(*this->cookie, &this->level, &this->player);
            levelPlay();
            break;

        case GameState::SeriesOver:
            completeSeries();
            break;

        default: break;
    
    }
	
}


// --------------------------------------------------------------------------------------
//  Display intro banner ..
//
void Game::splash() {
    
    if (Utils::isFrameCount(12)) {
     
        this->counter++;
        
        if (this->counter == 4) {
            
            this->counter = 0;
            
        }
        
    }
    
    PD::drawBitmap(38, 48, Images::Ppot[counter]);
    if (PC::buttons.pressed(BTN_A))  { this->gameState = GameState::Intro; }

}


// --------------------------------------------------------------------------------------
//  Display intro banner ..
//
void Game::intro() {

  PD::drawBitmap(0, 0, Images::Banner, false, false);

  bool firstTime = EEPROM_Utils::getMen(*this->cookie) == 5 && EEPROM_Utils::getLevelNumber(*this->cookie) == 1;

  if (firstTime) {

    this->menuSelect = 0;
    PD::drawBitmap(10, 165, Images::StartGame);

  }
  else {

    PD::drawBitmap(10, 165, Images::ResumeGame);
    PD::drawBitmap(120, 165, Images::RestartGame);

  }

  PD::drawBitmap(this->menuSelect == 0 ? 3 : 113, 165, Images::Arrow_Title);


  // Handle buttons ..

  if (!firstTime) {

    if (PC::buttons.pressed(BTN_LEFT) && this->menuSelect > 0)    { this->menuSelect--; }
    if (PC::buttons.pressed(BTN_RIGHT) && this->menuSelect < 1)   { this->menuSelect++; }

  }

  if (PC::buttons.pressed(BTN_A)) {
    
    if (this->menuSelect == 0) { 
      EEPROM_Utils::getSavedGameData(*this->cookie, &this->level, &this->player); 
      this->gameState = GameState::LevelInit; 
    }

    if (this->menuSelect == 1) { 
      EEPROM_Utils::initEEPROM(*this->cookie); 
      EEPROM_Utils::getSavedGameData(*this->cookie, &this->level, &this->player); 
      this->gameState = GameState::LevelInit; 
    }
     
  }

}


// --------------------------------------------------------------------------------------
//  Play the current level ..  
//
//  If 'play' is false, play is halted and the player flashes waiting on a keypress.
//
void Game::levelPlay() {

  uint8_t nearestX = getNearestX(0);
  uint8_t nearestY = getNearestY(0);


  if (gameState == GameState::LevelPlay) {

    LevelElement nearest = this->level.getLevelData(nearestX, nearestY);


    // Detect next movements for player and enemies ..

    playerMovements(nearestX, nearestY, nearest);


    if (Utils::isFrameCount(2)) {

      clearEnemyMovementPositions(enemies);
      for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

        Enemy *enemy = &enemies[x];

        if (enemy->getEnabled()) {

          enemyMovements(enemy);

        }

      }

    }

  }


  // Render the screen ..

  renderScreen();


  // Update the player and enemy stance, positions, etc ..

  if (this->gameState == GameState::LevelPlay) {



    // Update player stance ..

    if (Utils::isFrameCount(2)) {

      if ((this->player.getXDelta() != 0 || this->player.getYDelta() != 0 || this->level.getXOffsetDelta() != 0 || this->level.getYOffsetDelta() != 0)) {

        this->player.setStance(getNextStance(this->player.getStance()));

      }
    
    }
    if (Utils::isFrameCount(4)) {


      // Update enemy stances ..

      for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

        Enemy *enemy = &enemies[x];
        PlayerStance stance = enemy->getStance();

        if (enemy->getEnabled() && enemy->getEscapeHole() == EscapeHole::None) {

          switch (stance) {

            case PlayerStance::Rebirth_1 ... PlayerStance::Rebirth_3:
              
              enemy->setStance(getNextStance(stance));
              break;

            default:
              
              if (enemy->getXDelta() != 0 || enemy->getYDelta() != 0) {

                enemy->setStance(getNextStance(stance));

              }

              break;

          }

        }

      }

    }


    // Move player ..

    this->player.setX(this->player.getX() + this->player.getXDelta());
    this->player.setY(this->player.getY() + this->player.getYDelta());
    this->level.setXOffset(this->level.getXOffset() + this->level.getXOffsetDelta());
    this->level.setYOffset(this->level.getYOffset() + this->level.getYOffsetDelta());


    // If the player has gone off the top of the screen .. level over!

    LevelElement current = this->level.getLevelData(getNearestX(0), getNearestY(0));

    if (player.getY() <= 1 && current == LevelElement::Ladder) {

      uint8_t levelNumber = this->level.getLevelNumber() + 1;
      this->player.incrementMen();

      this->gameState = GameState::LevelExitInit;
      this->level.setLevelNumber(levelNumber);
      EEPROM_Utils::saveLevelNumber(*this->cookie, this->level.getLevelNumber());

      if (levelNumber > LEVEL_COUNT) {

        this->player.setNextState(GameState::SeriesOver);

      }
      else {
        this->player.setNextState(GameState::NextLevel);
      }

      //SJH sound.tones(levelComplete); 

    } 


    // Move enemies ..

    if (Utils::isFrameCount(2)) {

      for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

        Enemy *enemy = &enemies[x];

        if (enemy->getEnabled()) {

          enemy->setX(enemy->getX() + enemy->getXDelta());
          enemy->setY(enemy->getY() + enemy->getYDelta());

        }


        // Are any of the enemies touching the player?

        if (enemy->getEnabled() && Utils::collide(
            Rect {static_cast<int16_t>(enemy->getX()) + 2, static_cast<int16_t>(enemy->getY()) + 2, 6, 6}, 
            Rect {static_cast<int16_t>(this->player.getX() - level.getXOffset()) + 2, static_cast<int16_t>(this->player.getY() - this->level.getYOffset()) + 2, 6, 6} 
            )) {

          playerDies();

        }

      }


      // Update level details ..
      
      for (uint8_t y = 0; y < this->level.getHeight(); y++) {

        for (uint8_t x = 0; x < this->level.getWidth() * 2; x++) {

          LevelElement element = (LevelElement)this->level.getLevelData(x, y);
          
          switch (element) {

            case LevelElement::Brick_1 ... LevelElement::Brick_4:
              element++;
              this->level.setLevelData(x, y, element);
              break;

            default:
              break;

          }

        }

      }

    }


    // Do any holes need to be filled in ?

    if (!this->holes.isEmpty()) {

      for (uint8_t x = 0; x < this->holes.getCount(); x++) {

        Hole &hole = this->holes.operator[](x);

        if (hole.countDown > 0) {

          hole.countDown--;

          switch (hole.countDown) {

            case HOLE_FILL_4:        
              this->level.setLevelData(hole.x, hole.y, LevelElement::Brick_Close_1);
              break;

            case HOLE_FILL_3:
              this->level.setLevelData(hole.x, hole.y, LevelElement::Brick_Close_2);
              break;

            case HOLE_FILL_2:
              this->level.setLevelData(hole.x, hole.y, LevelElement::Brick_Close_3);
              break;

            case HOLE_FILL_1:
              this->level.setLevelData(hole.x, hole.y, LevelElement::Brick_Close_4);
              break;

            case 1:


              // Have any of the enemies been trapped ?  If so, relocate them ..

              for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

                Enemy *enemy = &enemies[x];

                if (enemy->getEnabled() && (hole.x * GRID_SIZE) == enemy->getX() && (hole.y * GRID_SIZE) == enemy->getY()) {

                  LevelPoint reentryPoint = this->level.getNextReentryPoint();
                  enemy->setX(reentryPoint.x * GRID_SIZE);
                  enemy->setY(reentryPoint.y * GRID_SIZE);
                  enemy->setStance( PlayerStance::Rebirth_1);
                  enemy->setEscapeHole(EscapeHole::None);
                  enemy->setXDelta(0);
                  enemy->setYDelta(0);

                }

              }


              // What about the player ?

              if ( hole.x == nearestX && hole.y == nearestY ) {

                  playerDies();

              }

              this->level.setLevelData(hole.x, hole.y, LevelElement::Brick);
              break;

            default: break;

          }

        }

      }


      // Burn any holes that have been filled in from the queue ..

      while (true) {
        
        Hole &hole = this->holes.peek();

        if (hole.countDown == 1) {

          this->holes.dequeue();

          if (this->holes.isEmpty()) { break; }

        }
        else {

          break;

        }

      }

    }

  }
  else {


    // Change level?

    if (gameState == GameState::LevelFlash) {

      if (PC::buttons.pressed(BTN_C) || PC::buttons.repeat(BTN_C, 1)) {

          switch (levelCount) {

            case 0 ... 30:
            
              this->levelCount++;
              this->levelShow = true;
              break;

            case 31 ... 35:

              this->levelShow = false;
              this->levelCount++;
              break;

            case 36 ... 40:

              this->levelShow = true;
              this->levelCount++;
              break;
                    
            default:

              uint8_t levelNumber = this->level.getLevelNumber();

              if (PC::buttons.pressed(BTN_UP) && levelNumber < LEVEL_COUNT) {
                this->level.setLevelNumber(levelNumber + 1);
              }
              else if (PC::buttons.pressed(BTN_DOWN) && levelNumber > 1) {
                this->level.setLevelNumber(levelNumber - 1);
              }
              else if (PC::buttons.repeat(BTN_UP, 15) && ((levelNumber / 10) * 10) < LEVEL_COUNT - 10) {
                this->level.setLevelNumber(((levelNumber / 10) * 10) + 10);
              }
              else if (PC::buttons.repeat(BTN_DOWN, 15) && ((levelNumber/10) * 10) > 10) {
                this->level.setLevelNumber(((levelNumber / 10) * 10) - 10);
              }

              break;

          }

      }
      else {

        switch (this->levelCount) {

          case 0:
            break;

          case 1 ... 40:
            this->levelCount = 0;
            this->levelShow = true;
            break;

          default:

            if (this->levelCount >= 40) {

              this->gameState = GameState::LevelInit;
              this->levelCount = 0;

            }
            else {

              this->levelCount = 0;

            }

            break;

        }

      }

    }
  

    // We are not playing so wait for a key press to continue the game ..

    if (this->levelCount == 0 && (PC::buttons.pressed(BTN_LEFT) || PC::buttons.pressed(BTN_RIGHT) || PC::buttons.pressed(BTN_UP) || PC::buttons.pressed(BTN_DOWN) || PC::buttons.pressed(BTN_A) || PC::buttons.pressed(BTN_B))) {

      switch (gameState) {

        // case GameState::NextGame:
        //   break;

        case GameState::NextLevel:
        case GameState::RestartLevel:
        this->gameState = GameState::LevelInit;  
          break;

        case GameState::GameOver:
          this->gameState = GameState::Intro;  
          break;

        case GameState::LevelExitAnimation:
          this->gameState = this->player.getNextState();
          break;

        default:
        //SJH arduboy.clearButtonState();
          this->gameState = GameState::LevelPlay;
          break;

      }  
      
    }

  }


  // Show level clear indicator?

  if (this->suicide == 0 && this->levelCount == 0) {
    //SJH arduboy.setRGBled(0, (this->level.getGoldLeft() == 0 && this->gameState == GameState::LevelPlay ? 32 : 0), 0);
  }

}


// --------------------------------------------------------------------------------------
//  Our player is dead ..
//
void Game::playerDies() {

  uint8_t menLeft = this->player.getMen() - 1;

  this->player.setMen(menLeft);
  this->gameState = GameState::LevelExitInit;

  if (menLeft > 0) {

    this->player.setNextState(GameState::RestartLevel);

  }
  else {

    this->player.setNextState(GameState::GameOver);

  }

  //SJH sound.tones(dead); 

}



// --------------------------------------------------------------------------------------
//  Display 'Next Game' banner ..
//
void Game::nextGame() {

  PD::drawBitmap(20, 23, Images::LoadNextGame);

}


// --------------------------------------------------------------------------------------
//  Display 'complete game' banner ..
//
// void Game::completeGame() {

//   uint8_t level = static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::CompleteGame1) + 1;
//   PD::drawBitmap(19, 20, Images::CompleteGame);
//   if (level == 1) PD::drawBitmap(71, 35, Images::CompleteGame1);
//   if (level == 2) PD::drawBitmap(71, 35, Images::CompleteGame2);
//   if (level == 3) PD::drawBitmap(71, 35, Images::CompleteGame3);

// }



// --------------------------------------------------------------------------------------
//  Display 'victory' banner ..
//
void Game::completeSeries() {

  PD::drawBitmap(29, 24, Images::Victory);

}
