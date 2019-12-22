#include "Game.h"
#include "Pokitto.h"

#include "characters/Enemy.h"
#include "characters/Player.h"
#include "src/sounds/Sounds.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;
using PS = Pokitto::Sound;


// ------------------------------------------------------------------------------------------
//  Stop player movement ..
// ------------------------------------------------------------------------------------------

void Game::stopPlayer() {

  this->player.setXDelta(0);
  this->level.setXOffsetDelta(0);   
  this->player.setYDelta(0);
  this->level.setYOffsetDelta(0); 

}


// ------------------------------------------------------------------------------------------
//  If the player is exactly within a grid 'cell', attemp to move them ..
// ------------------------------------------------------------------------------------------

void Game::playerMovements(uint8_t nearestX, uint8_t nearestY, LevelElement nearest) {


  // Stop the players running ahead ..

  this->player.setXDelta(0);
  this->level.setXOffsetDelta(0);

  if (player.getStance() != PlayerStance::Falling) {

    this->player.setYDelta(0);
    this->level.setYOffsetDelta(0);

  }



  // Check to see if we have touched gold!

  if (this->level.getLevelData(nearestX, nearestY) == LevelElement::Gold) {

    this->player.setScore(player.getScore() + 25);
    this->level.setLevelData(nearestX, nearestY, LevelElement::Blank);
    bool lastPiece = this->level.pickupGold();

    if (lastPiece) {
      this->goldFlash = true;
      PS::playSFX(Sounds::sfx_goldFinish, Sounds::sfx_goldFinish_length);
    }
    else {
      PS::playSFX(Sounds::sfx_getGold, Sounds::sfx_getGold_length);
    }

  }


  // Commit suicide?

  if (PC::buttons.pressed(BTN_C) || PC::buttons.repeat(BTN_C, 1)) {
      
    this->suicide++;

    switch (this->suicide) {

      case 61:
        this->suicide = 0;
        playerDies();
        return;

    }

  }
  else {

    switch (this->suicide) {

      case 0:
        break;

      case 1 ... 60:
        this->pause = true;
        this->suicide = 0;
        break;

    }
    
  }


  // Burn to the left .. 

  if (PC::buttons.pressed(BTN_B) && !PC::buttons.pressed(BTN_A)) {

    if (inCellY()) {

      nearestX = getNearestX(2);
      LevelElement left = this->level.getLevelData(nearestX - 1, nearestY);
      LevelElement leftDown = this->level.getLevelData(nearestX - 1, nearestY + 1);

      if (leftDown == LevelElement::Brick && !isSolid(left) && left != LevelElement::Ladder) {

        this->player.setStance(PlayerStance::Burn_Left);
        this->player.setXDelta(0);
        this->level.setXOffsetDelta(0);

        this->level.setLevelData(nearestX - 1, nearestY + 1, LevelElement::Brick_1);
        Hole hole = {static_cast<uint8_t>(nearestX - 1), static_cast<uint8_t>(nearestY + 1), HOLE_REFILL_TIME};
        this->holes.enqueue(hole);

        PC::buttons.pollButtons();
        PS::playSFX(Sounds::sfx_dig, Sounds::sfx_dig_length);
        return;

      }

    }

  } 


  // Burn to the right .. 

  else if (PC::buttons.pressed(BTN_A) && !PC::buttons.pressed(BTN_B)) {

    if (inCellY()) {

      nearestX = getNearestX(2);
      LevelElement right = this->level.getLevelData(nearestX + 1, nearestY);
      LevelElement rightDown = this->level.getLevelData(nearestX + 1, nearestY + 1);

      if (rightDown == LevelElement::Brick && !isSolid(right) && right != LevelElement::Ladder) {
 
        this->player.setStance(PlayerStance::Burn_Right);
        this->player.setXDelta(0);
        this->level.setXOffsetDelta(0);

        this->level.setLevelData(nearestX + 1, nearestY + 1, LevelElement::Brick_1);
        Hole hole = {static_cast<uint8_t>(nearestX + 1), static_cast<uint8_t>(nearestY + 1), HOLE_REFILL_TIME};
        this->holes.enqueue(hole);

        PC::buttons.pollButtons();
        PS::playSFX(Sounds::sfx_dig, Sounds::sfx_dig_length);
        return;

      }

    }

  }


  // ------------------------------------------------------------------------------------------
  //  Right
  
  if (PC::buttons.pressed(BTN_RIGHT) || PC::buttons.repeat(BTN_RIGHT, 1)) {

    bool moveRight = true;
    bool moveDown = false;

    LevelElement current = this->level.getLevelData(nearestX, nearestY);
    LevelElement rightUp = this->level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY - 1);
    LevelElement right = this->level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY);
    LevelElement rightDown = this->level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY + 1);
    LevelElement down = this->level.getLevelData((inCellX() ? nearestX : nearestX - 1), nearestY + 1);

    bool canBeOccupied_Right = canBeOccupied(right);
    bool canBeOccupied_RightUp = canBeOccupied(rightUp);
    bool canBeStoodOn_XY1 = canBeStoodOn(down, this->enemies, nearestX, nearestY + 1);
    bool inCell_X = inCellX();
    bool inCell_Y = inCellY();

    if (this->player.getStance() == PlayerStance::Falling) {

      if (inCell_Y && canBeStoodOn_XY1) {

        moveRight = false;
        moveDown = false;
        this->player.setStance(PlayerStance::StandingStill);
        PS::playSFX(nullptr, 0);

      }
      
      else if (inCell_Y && (right == LevelElement::Rail || current == LevelElement::Rail)) {

        updatePlayerStance(PlayerStance::Swinging_Right1, PlayerStance::Swinging_Right2);
        moveRight = true;
        PS::playSFX(nullptr, 0);

      }

      else { 

        moveRight = false;
        moveDown = true;

      }     

    }
    else if (inCellY(4)) {

      if (canBeClimbedOn(current)) {

        if (canBeOccupied_Right) {

          if (canBeStoodOn_XY1) {

            updatePlayerStance(PlayerStance::Running_Right1, PlayerStance::Running_Right4);

          }
          else {

            if (current == LevelElement::Rail)   { updatePlayerStance(PlayerStance::Swinging_Right1, PlayerStance::Swinging_Right4); }
            if (current == LevelElement::Ladder) { updatePlayerStance(PlayerStance::Climbing_Up1, PlayerStance::Climbing_Up2); }

          }

          moveRight = true;

        }
        else {

          moveRight = false;

        }

      }
      else if (inCell_X && canBeFallenInto(down, this->enemies, nearestX, nearestY + 1)) {

        this->player.setStance(PlayerStance::Falling);
        moveRight = false;
        moveDown = true;
        PS::playSFX(Sounds::sfx_trap, Sounds::sfx_trap_length);

      }
      else if (!canBeOccupied(right) && canBeStoodOn_XY1) {  

        this->player.setStance(PlayerStance::StandingStill);
        moveRight = false;

      }
      else {
          
        if (inCell_X) {

          if (canBeOccupied_Right) {

            updatePlayerStance(PlayerStance::Running_Right1, PlayerStance::Running_Right4);
            moveRight = true;

          }
          else {

            moveRight = false;

          }

        }
        else { // !inCellX()

          if (canBeOccupied_Right && canBeStoodOn(rightDown, this->enemies, nearestX + 1, nearestY + 1)) {

            updatePlayerStance(PlayerStance::Running_Right1, PlayerStance::Running_Right4);
            moveRight = true;

          }
          else if (canBeFallenInto(rightDown, this->enemies, nearestX + 1, nearestY + 1)) {

            updatePlayerStance(PlayerStance::Running_Right1, PlayerStance::Running_Right4);
            moveRight = true;

          }
          else {

            moveRight = false;

          }
          
        }
        
      }

    }
    else {

      rightUp = this->level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY);
      right = this->level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY + 1);
      rightDown = this->level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY + 2);
      down = this->level.getLevelData((inCellX() ? nearestX : nearestX + 1), nearestY + 2);

      canBeOccupied_Right = canBeOccupied(right);
      canBeOccupied_RightUp = canBeOccupied(rightUp);

      if (canBeOccupied_RightUp && canBeOccupied_Right && canBeStoodOn(rightDown, this->enemies, nearestX + 1, nearestY + 1)) {
        
        updatePlayerStance(PlayerStance::Running_Right1, PlayerStance::Running_Right4);
        this->player.setStance(PlayerStance::StandingStill);
        moveRight = true;

      }
      else if (inCell_X) {

        if (canBeOccupied_RightUp && canBeOccupied_Right && canBeFallenInto(rightDown, this->enemies, nearestX + 1, nearestY + 1)) {
          
          updatePlayerStance(PlayerStance::Climbing_Up1, PlayerStance::Climbing_Up2);
          moveRight = true;
          moveDown = false;
          PS::playSFX(nullptr, 0);

        }
        else {

          moveRight = false;

        }
        
      }
      else { // !inCellX()

        rightUp = this->level.getLevelData(nearestX, nearestY);
        right = this->level.getLevelData(nearestX, nearestY);
        rightDown = this->level.getLevelData(nearestX, nearestY + 1);
        down = this->level.getLevelData(nearestX - 1, nearestY + 1);

        canBeOccupied_Right = canBeOccupied(right);
        canBeOccupied_RightUp = canBeOccupied(rightUp);

        if (canBeOccupied_RightUp && canBeOccupied_Right && canBeFallenInto(rightDown, this->enemies, nearestX + 1, nearestY + 1)) {
          
          this->player.setStance(PlayerStance::Falling);        
          moveRight = true;
          moveDown = false;
          PS::playSFX(nullptr, 0);

        }
        else {

          moveRight = false;

        }

      }

    }

    
    // Move player if needed ..

    if (moveRight) {
      
      this->player.setY((nearestY * GRID_SIZE) + this->level.getYOffset());
      movePlayerRight();

    }
    else {
          
      this->player.setXDelta(0);
      this->level.setXOffsetDelta(0);    

    }

    if (moveDown) {

      if (this->player.getStance() != PlayerStance::Falling) {
        this->player.setStance(PlayerStance::Falling);
        PS::playSFX(Sounds::sfx_fall, Sounds::sfx_fall_length); 
      } 
      movePlayerDown();

    }
    else {
          
      this->player.setYDelta(0);
      this->level.setYOffsetDelta(0);    
      
    }

  }


  // ------------------------------------------------------------------------------------------
  //  Left
  
  else if (PC::buttons.pressed(BTN_LEFT) || PC::buttons.repeat(BTN_LEFT, 1)) {

    boolean moveLeft = true;
    boolean moveDown = false;

    LevelElement current = this->level.getLevelData(nearestX, nearestY);
    LevelElement leftUp = this->level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY - 1);
    LevelElement left = this->level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY);
    LevelElement leftDown = this->level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY + 1);
    LevelElement down = this->level.getLevelData((inCellX() ? nearestX : nearestX + 1), nearestY + 1);

    bool canBeOccupied_Left = canBeOccupied(left);
    bool canBeOccupied_LeftUp = canBeOccupied(leftUp);
    bool canBeStoodOn_XY1 = canBeStoodOn(down, this->enemies, nearestX, nearestY + 1);
    bool inCell_X = inCellX();
    bool inCell_Y = inCellY();

    if (player.getStance() == PlayerStance::Falling) {

      if (inCell_Y && canBeStoodOn_XY1) {

        moveLeft = false;
        moveDown = false;
        this->player.setStance(PlayerStance::StandingStill);
        PS::playSFX(nullptr, 0);

      }
      
      else if (inCell_Y && (left == LevelElement::Rail || current == LevelElement::Rail)) {

        updatePlayerStance(PlayerStance::Swinging_Left4, PlayerStance::Swinging_Left1);
        moveLeft = true;
        PS::playSFX(nullptr, 0);

      }

      else { 

        moveLeft = false;
        moveDown = true;
        
      }     

    }
    else if (inCellY(4)) {

      if (canBeClimbedOn(current)) {

        if (canBeOccupied_Left) {

          if (canBeStoodOn_XY1) {

            updatePlayerStance(PlayerStance::Running_Left4, PlayerStance::Running_Left1);

          }
          else {

            if (current == LevelElement::Rail)   { updatePlayerStance(PlayerStance::Swinging_Left4, PlayerStance::Swinging_Left1); }
            if (current == LevelElement::Ladder) { updatePlayerStance(PlayerStance::Climbing_Up1, PlayerStance::Climbing_Up2); }

          }
          moveLeft = true;

        }
        else {
          
          moveLeft = false;

        }

      }
      else if (inCell_X && canBeFallenInto(down, this->enemies, nearestX, nearestY + 1)) {

        this->player.setStance(PlayerStance::Falling);
        PS::playSFX(Sounds::sfx_fall, Sounds::sfx_fall_length); 
        moveLeft = false;
        moveDown = true;

      }      
      else if (!canBeOccupied_Left && canBeStoodOn(down, this->enemies, nearestX, nearestY + 1)) {  
       
        this->player.setStance(PlayerStance::StandingStill);
        moveLeft = false;

      }
      else {
          
        if (inCell_X) {

          if (canBeOccupied_Left) {

            updatePlayerStance(PlayerStance::Running_Left4, PlayerStance::Running_Left1);
            moveLeft = true;

          }
          else {

            moveLeft = false;

          }

        }
        else { // !inCellX()

          leftUp = this->level.getLevelData(nearestX, nearestY);
          left = this->level.getLevelData(nearestX, nearestY);
          leftDown = this->level.getLevelData(nearestX, nearestY + 1);
          down = this->level.getLevelData(nearestX + 1, nearestY + 1);
     
          canBeOccupied_Left = canBeOccupied(left);
          canBeOccupied_LeftUp = canBeOccupied(leftUp);
          canBeStoodOn_XY1 = canBeStoodOn(down, this->enemies, nearestX, nearestY + 1);

          if (canBeOccupied_Left && canBeStoodOn(leftDown, this->enemies, nearestX - 1, nearestY + 1)) {

            updatePlayerStance(PlayerStance::Running_Left4, PlayerStance::Running_Left1);
            moveLeft = true;

          }
          else if (canBeFallenInto(leftDown, this->enemies, nearestX - 1, nearestY + 1)) {

            updatePlayerStance(PlayerStance::Running_Left4, PlayerStance::Running_Left1);
            moveLeft = true;

          }
          else {

            moveLeft = false;

          }
          
        }
        
      }

    }
    else {

      leftUp = this->level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY);
      left = this->level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY + 1);
      leftDown = this->level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY + 2);
      down = this->level.getLevelData((inCellX() ? nearestX : nearestX - 1), nearestY + 2);

      canBeOccupied_Left = canBeOccupied(left);
      canBeOccupied_LeftUp = canBeOccupied(leftUp);
      
      if (canBeOccupied_LeftUp && canBeOccupied_Left && canBeStoodOn(leftDown, this->enemies, nearestX - 1, nearestY + 1)) {
        
        updatePlayerStance(PlayerStance::Running_Left4, PlayerStance::Running_Left1);
        this->player.setStance(PlayerStance::StandingStill);
        moveLeft = true;

      }
      else if (inCell_X) {

        if (canBeOccupied_LeftUp && canBeOccupied_Left && canBeFallenInto(leftDown, this->enemies, nearestX - 1, nearestY + 1)) {
          
          updatePlayerStance(PlayerStance::Climbing_Up1, PlayerStance::Climbing_Up2);
          moveLeft = true;
          moveDown = false;
          PS::playSFX(nullptr, 0);

        }
        else {

          moveLeft = false;

        }
        
      }
      else { // !inCellX()

        if (canBeOccupied_LeftUp && canBeOccupied_Left && canBeFallenInto(leftDown, this->enemies, nearestX - 1, nearestY + 1)) {
          
          this->player.setStance(PlayerStance::Falling);        
          moveLeft = true;
          moveDown = false;
          PS::playSFX(nullptr, 0);
        }
        else {

          moveLeft = false;

        }

      }

    }


    // Move player if needed ..

    if (moveLeft) {

      this->player.setY((nearestY * GRID_SIZE) + this->level.getYOffset());
      movePlayerLeft();

    }
    else {

      this->player.setXDelta(0);
      this->level.setXOffsetDelta(0);

    }

    if (moveDown) {

      if (this->player.getStance() != PlayerStance::Falling) {
        this->player.setStance(PlayerStance::Falling);
        PS::playSFX(Sounds::sfx_fall, Sounds::sfx_fall_length); 
      } 

      movePlayerDown();

    }
    else {
          
      this->player.setYDelta(0);
      this->level.setYOffsetDelta(0);    
      
    }

  }


  // ------------------------------------------------------------------------------------------
  //  Up

  else if (PC::buttons.pressed(BTN_UP) || PC::buttons.repeat(BTN_UP, 1)) {

    bool moveUp = true;
    bool moveDown = true;

    nearestY = getNearestY(+8);
    LevelElement up = this->level.getLevelData(nearestX, (inCellY() ? nearestY - 1 : nearestY));
    LevelElement down = this->level.getLevelData(nearestX, (inCellY() ? nearestY + 1 : nearestY));

    switch (player.getStance()) {

      case PlayerStance::Falling:

        moveUp = false;

        if (nearest == LevelElement::Rail && inCellY()) {

          this->player.setStance(PlayerStance::Swinging_Right1);
          moveDown = false;
          PS::playSFX(nullptr, 0);

        }
        else if (canBeStoodOn(down, this->enemies, nearestX, nearestY + 1)) {

          moveDown = false;
          this->player.setStance(PlayerStance::StandingStill);
          PS::playSFX(nullptr, 0);
          
        } 

        break;

      case PlayerStance::Climbing_Up1 ... PlayerStance::Climbing_Up2:

        if (inCellY() && (nearest != LevelElement::Ladder || isSolid(up))) { 

          moveUp = false;
          moveDown = false;
          PS::playSFX(nullptr, 0);

        }
        else {

          moveUp = true;

        }

        break;

      default:

        PS::playSFX(nullptr, 0);

        if (inCellX(4)) {

          if (nearest == LevelElement::Ladder && !isSolid(up)) {

            this->player.setX((nearestX * GRID_SIZE) + this->level.getXOffset());
            this->player.setStance(PlayerStance::Climbing_Up1);
            moveUp = true;
            moveDown = false;

          }
          else {

            moveUp = false;
            moveDown = false;

          }

        }
        else {

          moveUp = false;
          moveDown = false;

        }

        break;

    }


    // Move player if needed ..

    if (moveUp) {

      this->player.setX((nearestX * GRID_SIZE) + this->level.getXOffset());
      movePlayerUp();

    }
    else {

      if (!moveDown) {

        stopPlayer();

      }
      else {

        movePlayerDown();

      }

    }

  }


  // ------------------------------------------------------------------------------------------
  //  Down

  else if (PC::buttons.pressed(BTN_DOWN) || PC::buttons.repeat(BTN_DOWN, 1) || 
          ( !(PC::buttons.pressed(BTN_RIGHT) || PC::buttons.repeat(BTN_RIGHT, 1)) && !(PC::buttons.pressed(BTN_LEFT) || PC::buttons.repeat(BTN_LEFT, 1)) && !(PC::buttons.pressed(BTN_UP) || PC::buttons.repeat(BTN_UP, 1)) && 
          (this->player.getYDelta() == 2 || this->level.getYOffsetDelta() == -2) )) {

    boolean moveDown = true;

    nearestY = getNearestY(+8);
    LevelElement current = this->level.getLevelData(nearestX, (inCellY() ? nearestY : nearestY - 1));
    LevelElement down = this->level.getLevelData(nearestX, (inCellY() ? nearestY + 1 : nearestY));

    switch (player.getStance()) {

      case PlayerStance::Falling:

        if (nearest == LevelElement::Rail && inCellY()) {

          this->player.setStance(PlayerStance::Swinging_Right1);
          moveDown = false;
          PS::playSFX(nullptr, 0);

        }

        else if (inCellY() && current >= LevelElement::Brick_Close_1 && current <= LevelElement::Brick_Close_4) {

          moveDown = false;
          PS::playSFX(nullptr, 0);

        }

        else if (canBeStoodOn(down, this->enemies, nearestX, nearestY + 1)) {

          moveDown = false;
          this->player.setStance(PlayerStance::StandingStill);
          PS::playSFX(nullptr, 0);

        }          

        break;

      default:

        if (down == LevelElement::Ladder) {

          this->player.setX((nearestX * GRID_SIZE) + this->level.getXOffset());
          updatePlayerStance(PlayerStance::Climbing_Down2, PlayerStance::Climbing_Down1);
          moveDown = true;

        }
        else if (canBeOccupied(down)) {


          if (down == LevelElement::Blank) {
            if (this->player.getStance() != PlayerStance::Falling) {
              this->player.setStance(PlayerStance::Falling);
              PS::playSFX(Sounds::sfx_fall, Sounds::sfx_fall_length); 
            } 
          }
          this->player.setX((nearestX * GRID_SIZE) + this->level.getXOffset());
          moveDown = true;

        }
        else {

          moveDown = false;
          this->player.setStance(PlayerStance::StandingStill);
          PS::playSFX(nullptr, 0);

        }
        
        break;

    }


    // Move player if needed ..

    if (moveDown) {

      movePlayerDown();

    }
    else {

      stopPlayer();

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Move player or scenery left ..
// ------------------------------------------------------------------------------------------

void Game::movePlayerLeft() {

  this->player.setYDelta(0);
  this->level.setYOffsetDelta(0);

  if (player.getX() > /*60*/106) {
    
    this->player.setXDelta(-2);
    this->level.setXOffsetDelta(0);

  }
  else {

    if (this->level.getXOffset() < 0) {

      this->player.setXDelta(0);
      this->level.setXOffsetDelta(2);

    }
    else {

      if (this->player.getX() > 0) {

        this->player.setXDelta(-2);
        this->level.setXOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Move player or scenery right ..
// ------------------------------------------------------------------------------------------

void Game::movePlayerRight() {

  this->player.setYDelta(0);
  this->level.setYOffsetDelta(0);

  if (this->player.getX() < /*60*/106) {

    this->player.setXDelta(2);
    this->level.setXOffsetDelta(0);

  }
  else {

    if (this->level.getXOffset() > -((this->level.getWidth() * 2 * GRID_SIZE) - 220)) {

      this->player.setXDelta(0);
      this->level.setXOffsetDelta(-2);

    }
    else {

      if (this->player.getX() < 220) {

        this->player.setXDelta(2);
        this->level.setXOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Move player or scenery down ..
// ------------------------------------------------------------------------------------------

void Game::movePlayerDown() {

  this->player.setXDelta(0);
  this->level.setXOffsetDelta(0);

  this->player.setYDelta(2);
  this->level.setYOffsetDelta(0);


  if (this->player.getY() < (HEIGHT_LESS_TOOLBAR / 2) - 5) {

    this->player.setYDelta(2);
    this->level.setYOffsetDelta(0);

  }
  else {

    if (this->level.getYOffset() > -((this->level.getHeight() * GRID_SIZE) - HEIGHT_LESS_TOOLBAR)) {

      this->player.setYDelta(0);
      this->level.setYOffsetDelta(-2);

    }
    else {

      if (this->player.getY() < HEIGHT_LESS_TOOLBAR) {

        this->player.setYDelta(2);
        this->level.setYOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Move player or scenery up ..
// ------------------------------------------------------------------------------------------

void Game::movePlayerUp() {

  this->player.setXDelta(0);
  this->level.setXOffsetDelta(0);

  if (this->player.getY() > (HEIGHT_LESS_TOOLBAR / 2) - 5) {
    
    this->player.setYDelta(-2);
    this->level.setYOffsetDelta(0);

  }
  else {

    if (this->level.getYOffset() < 0) {

      this->player.setYDelta(0);
      this->level.setYOffsetDelta(2);

    }
    else {

      if (this->player.getY() > 0) {

        this->player.setYDelta(-2);
        this->level.setYOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Update player stance if appropriate ..
// ------------------------------------------------------------------------------------------

void Game::updatePlayerStance(PlayerStance lowerRange, PlayerStance upperRange) {

  if (this->player.getStance() < lowerRange || this->player.getStance() > upperRange) this->player.setStance(lowerRange);
  
}
  