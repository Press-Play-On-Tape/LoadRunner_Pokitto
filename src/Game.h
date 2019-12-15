#pragma once

#include "characters/Enemy.h"
#include "characters/Player.h"
#include "utils/Utils.h"
#include "utils/Queue.h"
#include "levels/Level.h"
#include "utils/GameCookie.h"

class Game {
    
    public:

        void setup(GameCookie *cookie);
        void loop(void);

        void splash();
        void intro();
        void gameSelect();
        void levelPlay();
        void playerDies();
        void nextGame();
        void completeGame();
        void completeSeries();

        void clearEnemyMovementPositions(Enemy *enemies);
        void enemyMovements(Enemy *enemy);
        boolean isOccupiedByAnotherEnemy(Enemy *enemies, Enemy *enemy, int8_t xDelta, int8_t yDelta);
        boolean attemptToMove(Enemy *enemy, uint8_t enemyX, uint8_t enemyY, Direction direction, LevelElement current, LevelElement up, LevelElement right, LevelElement rightDown, LevelElement down, LevelElement leftDown, LevelElement left, bool randomMoves);
        void setDirectionAfterHoleEscape(Enemy *enemy);
        void move(Enemy *enemy, int8_t x, int8_t y, bool randomMoves);
        void updateEnemyStance(Enemy *enemy, PlayerStance lowerRange, PlayerStance upperRange);

        void stopPlayer();
        void playerMovements(uint8_t nearestX, uint8_t nearestY, LevelElement nearest);
        void movePlayerLeft();
        void movePlayerRight();
        void movePlayerUp();
        void movePlayerDown();
        void updatePlayerStance(PlayerStance lowerRange, PlayerStance upperRange);

        void renderScreen();
        void renderLevelElements();
        void renderEnemies();
        void renderEntryRectangle();
        void renderScoreboard();

        uint8_t getNearestX(int8_t margin);
        uint8_t getNearestY(int8_t margin);
        boolean inCellX();
        boolean inCellX(uint8_t margin);
        boolean inCellY();
        boolean inCellY(uint8_t margin);
        PlayerStance getNextStance(PlayerStance stance);
        Direction getDirection_4Directions(Direction direction);
        Direction getDirection_16Directions(int16_t xDiff, int16_t yDiff);
        boolean canBeStoodOn(LevelElement levelElement, Enemy *enemies, uint16_t positionX, uint16_t positionY);
        boolean isSolid(LevelElement levelElement);
        boolean canBeFallenOn(LevelElement levelElement);
        boolean canBeClimbedOn(LevelElement levelElement);
        boolean canBeOccupied(LevelElement levelElement);
        boolean canBeFallenInto(LevelElement levelElement, Enemy *enemies, uint16_t positionX, uint16_t positionY);
        boolean canBeStoodOnBasic_Enemy(LevelElement levelElement);
        boolean canBeStoodOn_Enemy(LevelElement levelElement);
        boolean canBeOccupiedBasic_Enemy(LevelElement levelElement);
        boolean canBeOccupied_Enemy(LevelElement levelElement);
        boolean canContinueToFall_Enemy(LevelElement levelElement);
        boolean canBeFallenInto_Enemy(LevelElement levelElement, Enemy *enemies, uint16_t positionX, uint16_t positionY);
        

    private:

        Player player;
        Enemy enemies[NUMBER_OF_ENEMIES];
        
        Level level;
        GameCookie *cookie;
    
        bool flashPlayer = false;
        bool levelShow = true;
        
        GameState gameState = GameState::Splash;
        int8_t bannerStripe = -30;
        int8_t introRect = 0;
        Queue<Hole, 20> holes;
        
        uint8_t suicide = 0;
        uint8_t levelCount = 0;
        uint8_t menuSelect = 0;
        uint8_t counter;
        bool goldFlash;

};