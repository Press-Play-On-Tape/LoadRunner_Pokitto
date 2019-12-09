#pragma once

#include "Enums.h"
#include "GameCookie.h"

class EEPROM_Utils {

    public: 
        
        EEPROM_Utils() {};
        
        static void initEEPROM(GameCookie &cookie);
        static uint8_t getGameNumber(GameCookie &cookie);
        static uint8_t getLevelNumber(GameCookie &cookie);
        static uint8_t getMen(GameCookie &cookie);
        static void saveLevelNumber(GameCookie &cookie, uint8_t levelNumber);
        
        static void getSavedGameData(GameCookie &cookie, Level *level, Player *player);
        static void saveGameData(GameCookie &cookie, Level *level, Player *player);
        static void setGameNumber(GameCookie &cookie, uint8_t val);

};


/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised?
 */
void EEPROM_Utils::initEEPROM(GameCookie &cookie) {
    
    if (GAME_NUMBER == 1) {
        
        cookie.gameNumber = 1;
        cookie.levelNumber = 1;
        cookie.menLeft = 5;

        cookie.gameNumberOrig = 1;
        cookie.levelNumberOrig = 1;
        cookie.menLeftOrig = 5;
        cookie.scoreOrig = 0;

    }
    else {

        cookie.gameNumber = cookie.gameNumberOrig;        
        cookie.levelNumber = cookie.levelNumberOrig;        
        cookie.menLeft = cookie.menLeftOrig;        

    }

    cookie.saveCookie();

}


/* -----------------------------------------------------------------------------
 *   Get the current level number. 
 */
uint8_t EEPROM_Utils::getGameNumber(GameCookie &cookie) {

    return cookie.gameNumber;

}


/* -----------------------------------------------------------------------------
 *   Get the current level number. 
 */
uint8_t EEPROM_Utils::getLevelNumber(GameCookie &cookie) {

    return cookie.levelNumber;

}


/* -----------------------------------------------------------------------------
 *   Get the number of men remaining. 
 */
uint8_t EEPROM_Utils::getMen(GameCookie &cookie) {

    return cookie.menLeft;

}


/* -----------------------------------------------------------------------------
 *   Save the current level number. 
 */
void EEPROM_Utils::saveLevelNumber(GameCookie &cookie, uint8_t levelNumber) {

    cookie.levelNumber = levelNumber;
    cookie.saveCookie();

}


/* -----------------------------------------------------------------------------
 *   Get saved game data.
 */
void EEPROM_Utils::getSavedGameData(GameCookie &cookie, Level *level, Player *player) {

    level->setLevelNumber(cookie.levelNumber);
    player->setMen(cookie.menLeft);
    player->setScore(cookie.score);

}


/* -----------------------------------------------------------------------------
 *   Save game data.
 */
void EEPROM_Utils::saveGameData(GameCookie &cookie, Level *level, Player *player) {
 
    uint8_t levelNumber = level->getLevelNumber();
    cookie.levelNumber = levelNumber;
    cookie.levelNumberOrig = levelNumber;
    
    uint8_t menLeft = player->getMen();
    cookie.menLeft = menLeft;
    cookie.menLeftOrig = menLeft;
    
    uint16_t score = player->getScore();
    cookie.score = score;
    cookie.scoreOrig = score;

    cookie.saveCookie();

}


/* -----------------------------------------------------------------------------
 *   Save game number.
 */
void EEPROM_Utils::setGameNumber(GameCookie &cookie, uint8_t val) {

    cookie.gameNumber = val;
    cookie.gameNumberOrig = val;

    cookie.saveCookie();

}
