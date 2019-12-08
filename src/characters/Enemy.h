#pragma once

//#include "../utils/Utils.h"
#include "../utils/Enums.h"

class Enemy {

  public: 

    Enemy() {};
  
    uint8_t getId() const;
    uint16_t getX() const;
    uint16_t getY() const;
    int8_t getXDelta() const;
    int8_t getYDelta() const;
    int8_t getXFuturePosition() const;
    int8_t getYFuturePosition() const;
    bool getEnabled() const;
    uint8_t getGoldCountdown() const;
    uint8_t getDirectionCountdown() const;
    PlayerStance getStance() const;
    EscapeHole getEscapeHole() const;
    Direction getDirection() const;
    Direction getPreferredDirection() const;
    
    void setId(uint8_t val);
    void setX(uint16_t val);
    void setY(uint16_t val);
    void setXDelta(int8_t val);
    void setYDelta(int8_t val);
    void setXFuturePosition(int8_t val);
    void setYFuturePosition(int8_t val);
    void setEnabled(bool val);
    void setGoldCountdown(uint8_t val);
    void setDirectionCountdown(uint8_t val);
    void setStance(const PlayerStance &val);
    void setEscapeHole(const EscapeHole &val);
    void setDirection(const Direction &val);
    void setPreferredDirection(const Direction &val);
    void decrementDirectionCountdown(void);
    void decrementGoldCountdown(void);

  private:

  uint8_t _flags;           // bits 0 - 3 enemy id, bits 4 enabled
  uint8_t _futurePosition;  // bits 0 - 3 X, bits 4 - 7 Y
  uint16_t _x;
  uint16_t _y;
  int8_t _xDelta;
  int8_t _yDelta;
  uint8_t _goldCountdown;
  uint8_t _directionCountdown;

  PlayerStance _stance;
  EscapeHole _escapeHole;
  Direction _direction;
  Direction _preferredDirection;

};
