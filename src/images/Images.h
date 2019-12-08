#pragma once

namespace Images {
    
    #include "numbers/Number0.h"
    #include "numbers/Number1.h"
    #include "numbers/Number2.h"
    #include "numbers/Number3.h"
    #include "numbers/Number4.h"
    #include "numbers/Number5.h"
    #include "numbers/Number6.h"
    #include "numbers/Number7.h"
    #include "numbers/Number8.h"
    #include "numbers/Number9.h"

    const uint8_t* const Numbers[] = { 
        Number0, Number1, Number2, Number3, Number4, Number5, Number6, Number7, Number8, Number9,
    };


    #include "man/Man0.h"
    #include "man/Man1.h"
    #include "man/Man2.h"
    #include "man/Man3.h"
    #include "man/Man5.h"
    #include "man/Man6.h"
    #include "man/Man7.h"
    #include "man/Man8.h"
    #include "man/Man9.h"
    #include "man/Man11.h"
    #include "man/Man12Burn.h"
    #include "man/Man13Rebirth.h"
    #include "man/Man14.h"
    #include "man/Man15.h"

    const uint8_t* const Men[] = { 
//SJH        man0, man1, man2, man3, man2, man_LaddderLeft, man_LaddderRight, man7, man8, man7, man9, man11, man12, man13Rebirth, man14, man15
        Man0, Man1, Man2, Man3, Man2, Man2, Man2, Man7, Man8, Man7, Man9, Man11, Man12Burn, Man13Rebirth, Man14, Man15
    };
  
    #include "levelElements/Blank.h"
    #include "levelElements/Brick.h"
    #include "levelElements/Solid.h"
    #include "levelElements/Ladder.h"
    #include "levelElements/Rail.h"
    #include "levelElements/FallThrough.h"
    #include "levelElements/Gold.h"
    #include "levelElements/Brick_1.h"
    #include "levelElements/Brick_2.h"
    #include "levelElements/Brick_3.h"
    #include "levelElements/Brick_4.h"
    #include "levelElements/Brick_Close_1.h"
    #include "levelElements/Brick_Close_2.h"
    #include "levelElements/Brick_Close_3.h"
    #include "levelElements/Brick_Close_4.h"

    const uint8_t* const levelElementImgs[] = { 
        Blank, Brick, Solid, Ladder, Rail, FallThrough, Gold, Brick_1, Brick_2, Brick_3, Brick_4, Blank, Brick_Close_1, Brick_Close_2, Brick_Close_3, Brick_Close_4
    };

    #include "levelElements/Brick_1_Debri.h"
    #include "levelElements/Brick_2_Debri.h"
    #include "levelElements/Brick_3_Debri.h"
    #include "levelElements/Brick_4_Debri.h"

    const uint8_t* const brickDiggingImgs[] = {
        Brick_1_Debri, Brick_2_Debri, Brick_3_Debri, Brick_4_Debri
    };

    #include "complete/CompleteGame.h"
    #include "complete/CompleteGame1.h"
    #include "complete/CompleteGame2.h"
    #include "complete/CompleteGame3.h"
    #include "complete/CompleteGameBlank.h"
    #include "complete/CompleteGameBlank2.h"
    #include "complete/LoadNextGame.h"
    
    #include "Banner.h"
    #include "GameOver.h"
    #include "LevelChange.h"
    #include "Score.h"
    #include "Men_SC.h"
    #include "Level.h"
    #include "Gold_SC.h"
    #include "MenuOption.h"
    #include "MenuOption2.h"
    #include "MenuOptionStart.h"
    #include "TryAgain.h"
    #include "Victory.h"

    #include "arrows/Arrow.h"
    #include "arrows/ArrowTL.h"
    #include "arrows/ArrowTM.h"
    #include "arrows/ArrowTR.h"
    #include "arrows/ArrowBL.h"
    #include "arrows/ArrowBM.h"
    #include "arrows/ArrowBR.h"
    #include "arrows/ArrowML.h"
    #include "arrows/ArrowMR.h"
        
    #include "arrows/ArrowBL_Small.h"
    #include "arrows/ArrowBM_Small.h"
    #include "arrows/ArrowBR_Small.h"
    #include "arrows/ArrowTL_Small.h"
    #include "arrows/ArrowTM_Small.h"
    #include "arrows/ArrowTR_Small.h"
    #include "arrows/ArrowML_Small.h"
    #include "arrows/ArrowMR_Small.h"
        
}