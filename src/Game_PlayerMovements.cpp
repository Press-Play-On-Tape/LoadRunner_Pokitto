#include "Game.h"
#include "Pokitto.h"

#include "characters/Enemy.h"
#include "characters/Player.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


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

//   uint8_t justPressed =   arduboy.justPressedButtons();
//   uint8_t pressed =       arduboy.pressedButtons();


  // Stop the players running ahead ..

  this->player.setXDelta(0);
  this->level.setXOffsetDelta(0);

  if (player.getStance() != PlayerStance::Falling) {

    this->player.setYDelta(0);
    this->level.setYOffsetDelta(0);

  }



  // Check to see if we have touched gold!

  if (level.getLevelData(nearestX, nearestY) == LevelElement::Gold) {

    this->player.setScore(player.getScore() + 25);
    this->level.setLevelData(nearestX, nearestY, LevelElement::Blank);
    this->level.pickupGold();
    //SJH sound.tones(pickUpGold);

  }


  // Commit suicide?

//  if ((pressed & A_BUTTON) && (pressed & B_BUTTON)) {
  if (PC::buttons.pressed(BTN_A) && PC::buttons.pressed(BTN_B)) {
      
    this->suicide++;

    switch (suicide) {

      case 21 ... 60:
//SJH        arduboy.setRGBled(128 - (suicide * 2), 0, 0);
        break;

      case 61:
        this->suicide = 0;
//SJH        arduboy.setRGBled(0, 0, 0);
        playerDies();
        return;

    }

  }
  else {

    if (this->suicide > 0) {
    
//SJH      arduboy.setRGBled(0, 0, 0);
      this->suicide = 0;

    }
    
  }


  // Burn to the left .. 

//if ((justPressed & A_BUTTON) && !(justPressed & B_BUTTON)) {
  if (PC::buttons.pressed(BTN_A) && !PC::buttons.pressed(BTN_B)) {

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

        //SJH arduboy.pollButtons();
        //SJH sound.tones(digAHole);
        return;

      }

    }

  } 


  // Burn to the right .. 

//else if ((justPressed & B_BUTTON) && !(justPressed & A_BUTTON)) {
  else if (PC::buttons.pressed(BTN_B) && !PC::buttons.pressed(BTN_A)) {

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

        //SJH arduboy.pollButtons();
        //SJH sound.tones(digAHole);
        return;

      }

    }

  }


  // ------------------------------------------------------------------------------------------
  //  Right
  
//  if (pressed & RIGHT_BUTTON) {
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
    bool canBeStoodOn_XY1 = canBeStoodOn(down, enemies, nearestX, nearestY + 1);
    bool inCell_X = inCellX();
    bool inCell_Y = inCellY();

    if (this->player.getStance() == PlayerStance::Falling) {

      if (inCell_Y && canBeStoodOn_XY1) {

        moveRight = false;
        moveDown = false;
        this->player.setStance(PlayerStance::StandingStill);
        //SJH sound.noTone();

      }
      
      else if (inCell_Y && (right == LevelElement::Rail || current == LevelElement::Rail)) {

        updatePlayerStance(PlayerStance::Swinging_Right1, PlayerStance::Swinging_Right2);
        moveRight = true;
        //SJH sound.noTone();

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
      else if (inCell_X && canBeFallenInto(down, enemies, nearestX, nearestY + 1)) {

        this->player.setStance(PlayerStance::Falling);
        moveRight = false;
        moveDown = true;
        //SJH sound.tones(freeFalling);   

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

          if (canBeOccupied_Right && canBeStoodOn(rightDown, enemies, nearestX + 1, nearestY + 1)) {

            updatePlayerStance(PlayerStance::Running_Right1, PlayerStance::Running_Right4);
            moveRight = true;

          }
          else if (canBeFallenInto(rightDown, enemies, nearestX + 1, nearestY + 1)) {

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

      rightUp = level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY);
      right = level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY + 1);
      rightDown = level.getLevelData((inCellX() ? nearestX + 1 : nearestX), nearestY + 2);
      down = level.getLevelData((inCellX() ? nearestX : nearestX + 1), nearestY + 2);

      canBeOccupied_Right = canBeOccupied(right);
      canBeOccupied_RightUp = canBeOccupied(rightUp);

      if (canBeOccupied_RightUp && canBeOccupied_Right && canBeStoodOn(rightDown, enemies, nearestX + 1, nearestY + 1)) {
        
        updatePlayerStance(PlayerStance::Running_Right1, PlayerStance::Running_Right4);
        this->player.setStance(PlayerStance::StandingStill);
        moveRight = true;

      }
      else if (inCell_X) {

        if (canBeOccupied_RightUp && canBeOccupied_Right && canBeFallenInto(rightDown, enemies, nearestX + 1, nearestY + 1)) {
          
          updatePlayerStance(PlayerStance::Climbing_Up1, PlayerStance::Climbing_Up2);
          moveRight = true;
          moveDown = false;
          //SJH sound.noTone();

        }
        else {

          moveRight = false;

        }
        
      }
      else { // !inCellX()

        rightUp = level.getLevelData(nearestX, nearestY);
        right = level.getLevelData(nearestX, nearestY);
        rightDown = level.getLevelData(nearestX, nearestY + 1);
        down = level.getLevelData(nearestX - 1, nearestY + 1);

        canBeOccupied_Right = canBeOccupied(right);
        canBeOccupied_RightUp = canBeOccupied(rightUp);

        if (canBeOccupied_RightUp && canBeOccupied_Right && canBeFallenInto(rightDown, enemies, nearestX + 1, nearestY + 1)) {
          
          this->player.setStance(PlayerStance::Falling);        
          moveRight = true;
          moveDown = false;
          //SJH sound.noTone();

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

      this->player.setStance(PlayerStance::Falling);
      movePlayerDown();

    }
    else {
          
      this->player.setYDelta(0);
      this->level.setYOffsetDelta(0);    
      
    }

  }


  // ------------------------------------------------------------------------------------------
  //  Left
  
//else if (pressed & LEFT_BUTTON) {
  else if (PC::buttons.pressed(BTN_LEFT) || PC::buttons.repeat(BTN_LEFT, 1)) {

    boolean moveLeft = true;
    boolean moveDown = false;

    LevelElement current = level.getLevelData(nearestX, nearestY);
    LevelElement leftUp = level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY - 1);
    LevelElement left = level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY);
    LevelElement leftDown = level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY + 1);
    LevelElement down = level.getLevelData((inCellX() ? nearestX : nearestX + 1), nearestY + 1);

    bool canBeOccupied_Left = canBeOccupied(left);
    bool canBeOccupied_LeftUp = canBeOccupied(leftUp);
    bool canBeStoodOn_XY1 = canBeStoodOn(down, enemies, nearestX, nearestY + 1);
    bool inCell_X = inCellX();
    bool inCell_Y = inCellY();

    if (player.getStance() == PlayerStance::Falling) {

      if (inCell_Y && canBeStoodOn_XY1) {

        moveLeft = false;
        moveDown = false;
        player.setStance(PlayerStance::StandingStill);
        //SJH sound.noTone();

      }
      
      else if (inCell_Y && (left == LevelElement::Rail || current == LevelElement::Rail)) {

        updatePlayerStance(PlayerStance::Swinging_Left4, PlayerStance::Swinging_Left1);
        moveLeft = true;
        //SJH sound.noTone();

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
      else if (inCell_X && canBeFallenInto(down, enemies, nearestX, nearestY + 1)) {

        player.setStance(PlayerStance::Falling);
        //SJH sound.tones(freeFalling); 
        moveLeft = false;
        moveDown = true;

      }      
      else if (!canBeOccupied_Left && canBeStoodOn(down, enemies, nearestX, nearestY + 1)) {  
       
        player.setStance(PlayerStance::StandingStill);
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

          leftUp = level.getLevelData(nearestX, nearestY);
          left = level.getLevelData(nearestX, nearestY);
          leftDown = level.getLevelData(nearestX, nearestY + 1);
          down = level.getLevelData(nearestX + 1, nearestY + 1);
     
          canBeOccupied_Left = canBeOccupied(left);
          canBeOccupied_LeftUp = canBeOccupied(leftUp);
          canBeStoodOn_XY1 = canBeStoodOn(down, enemies, nearestX, nearestY + 1);

          if (canBeOccupied_Left && canBeStoodOn(leftDown, enemies, nearestX - 1, nearestY + 1)) {

            updatePlayerStance(PlayerStance::Running_Left4, PlayerStance::Running_Left1);
            moveLeft = true;

          }
          else if (canBeFallenInto(leftDown, enemies, nearestX - 1, nearestY + 1)) {

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

      leftUp = level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY);
      left = level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY + 1);
      leftDown = level.getLevelData((inCellX() ? nearestX - 1 : nearestX), nearestY + 2);
      down = level.getLevelData((inCellX() ? nearestX : nearestX - 1), nearestY + 2);

      canBeOccupied_Left = canBeOccupied(left);
      canBeOccupied_LeftUp = canBeOccupied(leftUp);
      
      if (canBeOccupied_LeftUp && canBeOccupied_Left && canBeStoodOn(leftDown, enemies, nearestX - 1, nearestY + 1)) {
        
        updatePlayerStance(PlayerStance::Running_Left4, PlayerStance::Running_Left1);
        player.setStance(PlayerStance::StandingStill);
        moveLeft = true;

      }
      else if (inCell_X) {

        if (canBeOccupied_LeftUp && canBeOccupied_Left && canBeFallenInto(leftDown, enemies, nearestX - 1, nearestY + 1)) {
          
          updatePlayerStance(PlayerStance::Climbing_Up1, PlayerStance::Climbing_Up2);
          moveLeft = true;
          moveDown = false;
          //SJH sound.noTone();

        }
        else {

          moveLeft = false;

        }
        
      }
      else { // !inCellX()

        if (canBeOccupied_LeftUp && canBeOccupied_Left && canBeFallenInto(leftDown, enemies, nearestX - 1, nearestY + 1)) {
          
          player.setStance(PlayerStance::Falling);        
          moveLeft = true;
          moveDown = false;
          //SJH sound.noTone();

        }
        else {

          moveLeft = false;

        }

      }

    }


    // Move player if needed ..

    if (moveLeft) {

      player.setY((nearestY * GRID_SIZE) + level.getYOffset());
      movePlayerLeft();

    }
    else {

      player.setXDelta(0);
      level.setXOffsetDelta(0);

    }

    if (moveDown) {

      player.setStance(PlayerStance::Falling);
      movePlayerDown();

    }
    else {
          
      player.setYDelta(0);
      level.setYOffsetDelta(0);    
      
    }

  }


  // ------------------------------------------------------------------------------------------
  //  Up

//else if (pressed & UP_BUTTON) {
  else if (PC::buttons.pressed(BTN_UP) || PC::buttons.repeat(BTN_UP, 1)) {

    bool moveUp = true;
    bool moveDown = true;

    nearestY = getNearestY(+8);
    LevelElement up = level.getLevelData(nearestX, (inCellY() ? nearestY - 1 : nearestY));
    LevelElement down = level.getLevelData(nearestX, (inCellY() ? nearestY + 1 : nearestY));

    switch (player.getStance()) {

      case PlayerStance::Falling:

        moveUp = false;

        if (nearest == LevelElement::Rail && inCellY()) {

          player.setStance(PlayerStance::Swinging_Right1);
          moveDown = false;
          //SJH sound.noTone();

        }
        else if (canBeStoodOn(down, enemies, nearestX, nearestY + 1)) {

          moveDown = false;
          player.setStance(PlayerStance::StandingStill);
          //SJH sound.noTone();
          
        } 

        break;

      case PlayerStance::Climbing_Up1 ... PlayerStance::Climbing_Up2:

        if (inCellY() && (nearest != LevelElement::Ladder || isSolid(up))) { 

          moveUp = false;
          moveDown = false;
          //SJH sound.noTone();

        }
        else {

          moveUp = true;

        }

        break;

      default:

        //SJH sound.noTone();

        if (inCellX(4)) {

          if (nearest == LevelElement::Ladder && !isSolid(up)) {

            player.setX((nearestX * GRID_SIZE) + level.getXOffset());
            player.setStance(PlayerStance::Climbing_Up1);
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

      player.setX((nearestX * GRID_SIZE) + level.getXOffset());
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

//  else if ((pressed & DOWN_BUTTON) || ( !(pressed & RIGHT_BUTTON) && !(pressed & LEFT_BUTTON) && !(pressed & UP_BUTTON) && (player.getYDelta() == 2 || level.getYOffsetDelta() == -2) )) {
  else if (PC::buttons.pressed(BTN_DOWN) || PC::buttons.repeat(BTN_DOWN, 1) || 
          ( !(PC::buttons.pressed(BTN_RIGHT) || PC::buttons.repeat(BTN_RIGHT, 1)) && !(PC::buttons.pressed(BTN_LEFT) || PC::buttons.repeat(BTN_LEFT, 1)) && !(PC::buttons.pressed(BTN_UP) || PC::buttons.repeat(BTN_UP, 1)) && 
          (this->player.getYDelta() == 2 || this->level.getYOffsetDelta() == -2) )) {

    boolean moveDown = true;

    nearestY = getNearestY(+8);
    LevelElement current = level.getLevelData(nearestX, (inCellY() ? nearestY : nearestY - 1));
    LevelElement down = level.getLevelData(nearestX, (inCellY() ? nearestY + 1 : nearestY));

    switch (player.getStance()) {

      case PlayerStance::Falling:

        if (nearest == LevelElement::Rail && inCellY()) {

          player.setStance(PlayerStance::Swinging_Right1);
          moveDown = false;
          //SJH sound.noTone();

        }

        else if (inCellY() && current >= LevelElement::Brick_Close_1 && current <= LevelElement::Brick_Close_4) {

          moveDown = false;
          //SJH sound.noTone();

        }

        else if (canBeStoodOn(down, enemies, nearestX, nearestY + 1)) {

          moveDown = false;
          player.setStance(PlayerStance::StandingStill);
          //SJH sound.noTone();

        }          

        break;

      default:

        if (down == LevelElement::Ladder) {

          player.setX((nearestX * GRID_SIZE) + level.getXOffset());
          updatePlayerStance(PlayerStance::Climbing_Down2, PlayerStance::Climbing_Down1);
          moveDown = true;

        }
        else if (canBeOccupied(down)) {

          if (down == LevelElement::Blank) player.setStance(PlayerStance::Falling);
          player.setX((nearestX * GRID_SIZE) + level.getXOffset());
          moveDown = true;

        }
        else {

          moveDown = false;
          player.setStance(PlayerStance::StandingStill);
          //SJH sound.noTone();

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

  player.setYDelta(0);
  level.setYOffsetDelta(0);

  if (player.getX() > 60) {
    
    player.setXDelta(-2);
    level.setXOffsetDelta(0);

  }
  else {

    if (level.getXOffset() < 0) {

      player.setXDelta(0);
      level.setXOffsetDelta(2);

    }
    else {

      if (player.getX() > 0) {

        player.setXDelta(-2);
        level.setXOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Move player or scenery right ..
// ------------------------------------------------------------------------------------------

void Game::movePlayerRight() {

  player.setYDelta(0);
  level.setYOffsetDelta(0);

  if (player.getX() < 60) {

    player.setXDelta(2);
    level.setXOffsetDelta(0);

  }
  else {

    if (level.getXOffset() > -((level.getWidth() * 2 * GRID_SIZE) - 128/*WIDTH*/)) {

      player.setXDelta(0);
      level.setXOffsetDelta(-2);

    }
    else {

      if (player.getX() < 128) {

        player.setXDelta(2);
        level.setXOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Move player or scenery down ..
// ------------------------------------------------------------------------------------------

void Game::movePlayerDown() {

  player.setXDelta(0);
  level.setXOffsetDelta(0);

  player.setYDelta(2);
  level.setYOffsetDelta(0);


  if (player.getY() < (HEIGHT_LESS_TOOLBAR / 2) - 5) {

    player.setYDelta(2);
    level.setYOffsetDelta(0);

  }
  else {

    if (level.getYOffset() > -((level.getHeight() * GRID_SIZE) - HEIGHT_LESS_TOOLBAR)) {

      player.setYDelta(0);
      level.setYOffsetDelta(-2);

    }
    else {

      if (player.getY() < HEIGHT_LESS_TOOLBAR) {

        player.setYDelta(2);
        level.setYOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Move player or scenery up ..
// ------------------------------------------------------------------------------------------

void Game::movePlayerUp() {

  player.setXDelta(0);
  level.setXOffsetDelta(0);

  if (player.getY() > (HEIGHT_LESS_TOOLBAR / 2) - 5) {
    
    player.setYDelta(-2);
    level.setYOffsetDelta(0);

  }
  else {

    if (level.getYOffset() < 0) {

      player.setYDelta(0);
      level.setYOffsetDelta(2);

    }
    else {

      if (player.getY() > 0) {

        player.setYDelta(-2);
        level.setYOffsetDelta(0);

      }

    }

  }

}


// ------------------------------------------------------------------------------------------
//  Update player stance if appropriate ..
// ------------------------------------------------------------------------------------------

void Game::updatePlayerStance(PlayerStance lowerRange, PlayerStance upperRange) {

  if (player.getStance() < lowerRange || player.getStance() > upperRange) player.setStance(lowerRange);
  
}
  