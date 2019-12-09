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

};


/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised?
 */
void EEPROM_Utils::initEEPROM(GameCookie &cookie) {
    
    cookie.gameNumber = 1;
    cookie.levelNumber = 1;
    cookie.menLeft = 5;

    // cookie.gameNumberOrig = 1;
    // cookie.levelNumberOrig = 1;
    // cookie.menLeftOrig = 5;
    // cookie.scoreOrig = 0;

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
