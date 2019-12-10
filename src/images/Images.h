#pragma once

namespace Images {

    #include "Banner.h"
    #include "StartGame.h"
    #include "ResumeGame.h"
    #include "RestartGame.h"
    #include "Arrow_Title.h"
    
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

    #include "menus/Level_Seque.h"
    #include "menus/Number0_Seque.h"
    #include "menus/Number1_Seque.h"
    #include "menus/Number2_Seque.h"
    #include "menus/Number3_Seque.h"
    #include "menus/Number4_Seque.h"
    #include "menus/Number5_Seque.h"
    #include "menus/Number6_Seque.h"
    #include "menus/Number7_Seque.h"
    #include "menus/Number8_Seque.h"
    #include "menus/Number9_Seque.h"

    const uint8_t* const Numbers_Seque[] = { 
        Number0_Seque, Number1_Seque, Number2_Seque, Number3_Seque, Number4_Seque, Number5_Seque, Number6_Seque, Number7_Seque, Number8_Seque, Number9_Seque
    };

    #include "scoreboard/Number0_SC.h"
    #include "scoreboard/Number1_SC.h"
    #include "scoreboard/Number2_SC.h"
    #include "scoreboard/Number3_SC.h"
    #include "scoreboard/Number4_SC.h"
    #include "scoreboard/Number5_SC.h"
    #include "scoreboard/Number6_SC.h"
    #include "scoreboard/Number7_SC.h"
    #include "scoreboard/Number8_SC.h"
    #include "scoreboard/Number9_SC.h"

    const uint8_t* const Numbers_SC[] = { 
        Number0_SC, Number1_SC, Number2_SC, Number3_SC, Number4_SC, Number5_SC, Number6_SC, Number7_SC, Number8_SC, Number9_SC,
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
        Man0, Man1, Man2, Man3, Man2, Man5, Man6, Man7, Man8, Man7, Man9, Man11, Man12Burn, Man13Rebirth, Man14, Man15
    };

    #include "enemy/Enemy0.h"
    #include "enemy/Enemy1.h"
    #include "enemy/Enemy2.h"
    #include "enemy/Enemy3.h"
    #include "enemy/Enemy5.h"
    #include "enemy/Enemy6.h"
    #include "enemy/Enemy7.h"
    #include "enemy/Enemy8.h"
    #include "enemy/Enemy9.h"
    #include "enemy/Enemy11.h"
    #include "enemy/Enemy12Burn.h"
    #include "enemy/Enemy13Rebirth.h"
    #include "enemy/Enemy14.h"
    #include "enemy/Enemy15.h"

    const uint8_t* const Enemy[] = { 
        Enemy0, Enemy1, Enemy2, Enemy3, Enemy2, Enemy5, Enemy6, Enemy7, Enemy8, Enemy7, Enemy9, Enemy11, Enemy12Burn, Enemy13Rebirth, Enemy14, Enemy15
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

    #include "scoreboard/Score_SC.h"
    #include "scoreboard/Men_SC.h"
    #include "scoreboard/Level_SC.h"
    #include "scoreboard/Gold_SC.h"
    
    #include "menus/GameOver.h"
    #include "menus/TryAgain.h"
    #include "menus/Victory.h"

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
        
    #include "ppot/Ppot_00.h"
    #include "ppot/Ppot_01.h"
    #include "ppot/Ppot_02.h"
    #include "ppot/Ppot_03.h"

    const uint8_t * const Ppot[] = { 
        
        Ppot_00, Ppot_01, Ppot_02, Ppot_03,
        
    };
}