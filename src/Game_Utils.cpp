#include "Game.h"

#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


uint8_t Game::getNearestX(int8_t margin) {

  return (player.getX() - level.getXOffset() + margin) / GRID_SIZE;
  
}

uint8_t Game::getNearestY(int8_t margin) {

  return (player.getY() - level.getYOffset() + margin) / GRID_SIZE;
  
}

boolean Game::inCellX() {

  return ((player.getX() - level.getXOffset()) % GRID_SIZE == 0);

}

boolean Game::inCellX(uint8_t margin) {

  uint8_t rem = (player.getX() - level.getXOffset()) % GRID_SIZE;
  return (rem > (GRID_SIZE / 2) ? GRID_SIZE - rem : rem) <= margin;

}

boolean Game::inCellY() {

  return ((player.getY() - level.getYOffset()) % GRID_SIZE == 0);

}

boolean Game::inCellY(uint8_t margin) {

  uint8_t rem = (player.getY() - level.getYOffset()) % GRID_SIZE;
  return ((rem > (GRID_SIZE / 2) ? GRID_SIZE - rem : rem) <= margin);

}

const PlayerStance nextStances[] = {
    PlayerStance::Burn_Left, // -12
    static_cast<PlayerStance>(-11),
    PlayerStance::Swinging_Left1,
    PlayerStance::Swinging_Left4,
    PlayerStance::Swinging_Left3,
    PlayerStance::Swinging_Left2,
    PlayerStance::Climbing_Down1,
    PlayerStance::Climbing_Down2,
    PlayerStance::Running_Left1,
    PlayerStance::Running_Left4,
    PlayerStance::Running_Left3,
    PlayerStance::Running_Left2,
    PlayerStance::StandingStill,	
    PlayerStance::Running_Right2,
    PlayerStance::Running_Right3,
    PlayerStance::Running_Right4,
    PlayerStance::Running_Right1,
    PlayerStance::Climbing_Up2,
    PlayerStance::Climbing_Up1,
    PlayerStance::Swinging_Right2,
    PlayerStance::Swinging_Right3,
    PlayerStance::Swinging_Right4,
    PlayerStance::Swinging_Right1,
    PlayerStance::Falling,
    PlayerStance::Burn_Right,
    PlayerStance::Rebirth_2,
    PlayerStance::Rebirth_3,
    PlayerStance::StandingStill,
};


PlayerStance Game::getNextStance(PlayerStance stance) {

  uint8_t index = static_cast<uint8_t>(stance) + 12;
  if (index > 27) return stance;
//  return static_cast<PlayerStance>(pgm_read_byte(&nextStances[index]));
  return static_cast<PlayerStance>(nextStances[index]);

}


Direction Game::getDirection_4Directions(Direction direction) {

  switch (direction) {

    case Direction::RightUp2 ... Direction::RightDown:    return Direction::Right;
    case Direction::RightDown2 ... Direction::LeftDown:   return  Direction::Down;
    case Direction::LeftDown2 ... Direction::LeftUp:      return  Direction::Left;
    default:                                              return Direction::Up;

  }

}


Direction Game::getDirection_16Directions(int16_t xDiff, int16_t yDiff) {

  if (xDiff < 0) {
  
    if (yDiff > 0) {
    
      if (absT(xDiff) > absT(yDiff))          { return Direction::RightUp2; }
      else if (absT(xDiff) < absT(yDiff))     { return Direction::RightUp1; }
      else                                    { return Direction::RightUp; }
      
    }
    else if (yDiff < 0) {
    
      if (absT(xDiff) < absT(yDiff))          { return Direction::RightDown2; }
      else if (absT(xDiff) > absT(yDiff))     { return Direction::RightDown1; }
      else                                    { return Direction::RightDown; }
    
    }
    else {
    
      return Direction::Right;
      
    }
  
  } 
  else if (xDiff > 0) {
  
    if (yDiff > 0) {
    
      if (absT(xDiff) > absT(yDiff))          { return Direction::LeftUp1; }
      else if (absT(xDiff) < absT(yDiff))     { return Direction::LeftUp2; }
      else                                    { return Direction::LeftUp; }
      
    }
    else if (yDiff < 0) {
    
      if (absT(xDiff) < absT(yDiff))          { return Direction::LeftDown1; }
      else if (absT(xDiff) > absT(yDiff))     { return Direction::LeftDown2; }
      else                                    { return Direction::LeftDown; }
    
    }
    else {
    
      return Direction::Left;
      
    }
  
  }
  else {  
  
    if (yDiff < 0) {
    
      return Direction::Down;
      
    }
    else if (yDiff > 0) {
    
      return Direction::Up;
      
    }
    else {
    
      return Direction::Up;   // Player should be dead !
      
    }
  
  }

  return Direction::Up;       // Default, should never get here!

}



// ---------------------------------------------------------------------------------
// Can be stood on by the player ?
//
// The player cannot stand on brick that has been dug out or is repairing itself.
//
boolean Game::canBeStoodOn(LevelElement levelElement, Enemy *enemies, uint16_t positionX, uint16_t positionY) {

  switch (levelElement) {

    case LevelElement::Brick:
    case LevelElement::Solid:
    case LevelElement::Ladder:
      return true;

    default:


	  // Is the player standing on top of an enemy?

      for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

        Enemy *enemy = &enemies[x];

        if (enemy->getEnabled()) {

          if (enemy->getX() == positionX * GRID_SIZE && enemy->getY() == positionY * GRID_SIZE) {

            return true; 

          }

        }

      }

	    return false;

  }

}


// ---------------------------------------------------------------------------------
// Can be stood on by the enemy ?
//
boolean Game::isSolid(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Brick:
    case LevelElement::Solid:
      return true;

    default:
      return false;

  }

}

// ---------------------------------------------------------------------------------
// Can the player fall onto these elements ?
//
// Elements should be the same as the 'canBeStoodOn' with the addition of the rail.
//
boolean Game::canBeFallenOn(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Brick:
    case LevelElement::Solid:
    case LevelElement::Rail:
    case LevelElement::Ladder:
      return true;

    default:
      return false;

  }

}


// ---------------------------------------------------------------------------------
// Can the space be climbed on by the player ?
//
boolean Game::canBeClimbedOn(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Ladder:
    case LevelElement::Rail:
      return true;

    default:
      return false;

  }

}


// ---------------------------------------------------------------------------------
// Can the space be occupied by the player ?
//
// Can the player enter the square - ie. is it empty or something that can be climbed on?
//
boolean Game::canBeOccupied(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Blank:
    case LevelElement::Ladder:
    case LevelElement::Rail:
    case LevelElement::Gold:
    case LevelElement::FallThrough:
    case LevelElement::Brick_1 ... LevelElement::Brick_Close_4:
      return true;

    default:
      return false;

  }

}


// ---------------------------------------------------------------------------------
// Can the space be fallen into by the player ?
//
boolean Game::canBeFallenInto(LevelElement levelElement, Enemy *enemies, uint16_t positionX, uint16_t positionY) {

  switch (levelElement) {

    case LevelElement::Blank:
    case LevelElement::FallThrough:
    case LevelElement::Rail:
    case LevelElement::Gold:
      return true;

    case LevelElement::Brick_1:
    case LevelElement::Brick_2:
    case LevelElement::Brick_3:
    case LevelElement::Brick_4:
    case LevelElement::Brick_Transition:
    case LevelElement::Brick_Close_1:
    case LevelElement::Brick_Close_2:
    case LevelElement::Brick_Close_3:
    case LevelElement::Brick_Close_4:

      for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

        Enemy *enemy = &enemies[x];

        if (enemy->getEnabled()) {

          if (enemy->getStance() == PlayerStance::Falling && enemy->getX() == positionX * GRID_SIZE && enemy->getY() == positionY * GRID_SIZE) {

            return false; 

          }

        }

      }

      return true;

    default:
      return false;

  }

}

// ---------------------------------------------------------------------------------
// Can be stood on by the enemy ?
//
boolean Game::canBeStoodOnBasic_Enemy(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Brick:
    case LevelElement::Solid:
    case LevelElement::Ladder:
      return true;

    default:
      return false;

  }

}

// ---------------------------------------------------------------------------------
// Can be stood on by the enemy ?
//
// The enemy can stand on bricks that have been dug out or are reforming.  Of 
// course they will fall into these if they do step on them.
//
boolean Game::canBeStoodOn_Enemy(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Brick:
    case LevelElement::Solid:
    case LevelElement::Ladder:
    case LevelElement::Brick_1:
    case LevelElement::Brick_2:
    case LevelElement::Brick_3:
    case LevelElement::Brick_4:
    case LevelElement::Brick_Transition:
    case LevelElement::Brick_Close_1:
    case LevelElement::Brick_Close_2:
    case LevelElement::Brick_Close_3:
    case LevelElement::Brick_Close_4:
      return true;

    default:
      return false;

  }

}


// ---------------------------------------------------------------------------------
// Can the space be occupied by the enemy ?
//
// Can the player enter the square - ie. is it empty or something that can be climbed on?
//
boolean Game::canBeOccupiedBasic_Enemy(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Blank:
    case LevelElement::Ladder:
    case LevelElement::Rail:
      return true;

    default:
      return false;

  }

}


// ---------------------------------------------------------------------------------
// Can the space be occupied by the enemy ?
//
// Can the player enter the square - ie. is it empty or something that can be climbed on?
//
boolean Game::canBeOccupied_Enemy(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Blank:
    case LevelElement::Ladder:
    case LevelElement::Rail:
    case LevelElement::FallThrough:
    case LevelElement::Gold:
    case LevelElement::Brick_1:
    case LevelElement::Brick_2:
    case LevelElement::Brick_3:
    case LevelElement::Brick_4:
    case LevelElement::Brick_Transition:
    case LevelElement::Brick_Close_1:
    case LevelElement::Brick_Close_2:
    case LevelElement::Brick_Close_3:
    case LevelElement::Brick_Close_4:
      return true;

    default:
      return false;

  }

}


// ---------------------------------------------------------------------------------
// Can a falling enemy continue to fall ?  Should stop if they have fallen into a dug hole.
//
boolean Game::canContinueToFall_Enemy(LevelElement levelElement) {

  switch (levelElement) {

    case LevelElement::Blank:
    case LevelElement::FallThrough:
      return true;

    default:
      return false;

  }

}


// ---------------------------------------------------------------------------------
// Can the space be fallen into by the enemy ?
//
boolean Game::canBeFallenInto_Enemy(LevelElement levelElement, Enemy *enemies, uint16_t positionX, uint16_t positionY) {

  switch (levelElement) {

    case LevelElement::Blank:
    case LevelElement::FallThrough:
      return true;

    case LevelElement::Brick_1:
    case LevelElement::Brick_2:
    case LevelElement::Brick_3:
    case LevelElement::Brick_4:
    case LevelElement::Brick_Transition:
    case LevelElement::Brick_Close_1:
    case LevelElement::Brick_Close_2:
    case LevelElement::Brick_Close_3:
//  case LevelElement::Brick_Close_4:


      // Is another enemy in the position already ?

      for (uint8_t x = 0; x < NUMBER_OF_ENEMIES; x++) {

        Enemy *enemy = &enemies[x];

        if (enemy->getEnabled()) {

          if (enemy->getX() == positionX * GRID_SIZE && enemy->getY() == positionY * GRID_SIZE) {

            return false; 

          }

        }

      }

      return true;

    default:
      return false;

  }

}
