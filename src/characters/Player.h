#pragma once

//#include "../utils/Utils.h"
#include "../utils/Enums.h"

class Player {

  public: 

    Player() {};
  
    uint8_t getX();
    uint8_t getY();
    int8_t getXDelta();
    int8_t getYDelta();
    uint16_t getScore();
    uint8_t getMen();
    PlayerStance getStance();
    GameState getNextState();
    
    void setX(uint8_t val);
    void setY(uint8_t val);
    void setXDelta(int8_t val);
    void setYDelta(int8_t val);
    void setScore(uint16_t val);
    void setMen(uint8_t val);
    void setStance(const PlayerStance val);
    void setNextState(const GameState val);
    void incrementMen();

  private:

    uint8_t _x;
    uint8_t _y;
    int8_t _xDelta;
    int8_t _yDelta;
    uint16_t _score;
    uint8_t _men;
    PlayerStance _stance;
    GameState _nextState;

};
