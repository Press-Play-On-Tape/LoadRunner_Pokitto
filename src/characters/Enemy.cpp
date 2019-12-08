#include "Enemy.h"


//--------------------------------------------------------------------------------------------------------------------------

uint8_t Enemy::getId() const {
  return (_flags & 0x0f);
}

uint16_t Enemy::getX() const {
  return _x;
}

uint16_t Enemy::getY() const {
  return _y;
}

int8_t Enemy::getXDelta() const {
  return _xDelta;
}

int8_t Enemy::getYDelta() const {
  return _yDelta;
}

int8_t Enemy::getXFuturePosition() const {
  return (_futurePosition & 0x0f);
}

int8_t Enemy::getYFuturePosition() const {
  return (_futurePosition & 0xF0) >> 4;
}

bool Enemy::getEnabled() const {
  return (_flags & 0x10) == 0x10;
}

uint8_t Enemy::getGoldCountdown() const {
  return _goldCountdown;
}

uint8_t Enemy::getDirectionCountdown() const {
  return _directionCountdown;
}

PlayerStance Enemy::getStance() const {
  return _stance;
}

EscapeHole Enemy::getEscapeHole() const {
  return _escapeHole;
}

Direction Enemy::getDirection() const {
  return _direction;
}

Direction Enemy::getPreferredDirection() const {
  return _preferredDirection;
}

void Enemy::setId(uint8_t val) {
  _flags = (_flags & 0xf0) | val;
}
    
void Enemy::setX(uint16_t val) {
  _x = val;
}

void Enemy::setY(uint16_t val) {
  _y = val;
}

void Enemy::setXDelta(int8_t val) {
  _xDelta = val;
}

void Enemy::setYDelta(int8_t val) {
  _yDelta = val;
}

void Enemy::setXFuturePosition(int8_t val) {
  _futurePosition = (_futurePosition & 0xf0) | val;
}

void Enemy::setYFuturePosition(int8_t val) {
  _futurePosition = (_futurePosition & 0x0f) | (val << 4);
}

void Enemy::setEnabled(bool val) {
  _flags = (_flags | (val ? 0x10 : 0x00));
}

void Enemy::setGoldCountdown(uint8_t val) {
  _goldCountdown = val;
}

void Enemy::setDirectionCountdown(uint8_t val) {
  _directionCountdown = val;
}

void Enemy::setStance(const PlayerStance &val) {
  _stance = val;
}

void Enemy::setEscapeHole(const EscapeHole &val) {
  _escapeHole = val;
}

void Enemy::setDirection(const Direction &val) {
  _direction = val;
}

void Enemy::setPreferredDirection(const Direction &val) {
  _preferredDirection = val;
}

void Enemy::decrementDirectionCountdown(void) {
  --this->_directionCountdown;
}

void Enemy::decrementGoldCountdown(void) {
  --this->_goldCountdown;
}


