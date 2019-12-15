#include "Pokitto.h"
#include "PokittoCookie.h"
#include "src/Game.h"
#include "src/utils/GameCookie.h"
#include "src/sounds/Sounds.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;
using PS = Pokitto::Sound;

Game game;
GameCookie cookie;

int main() {


    // Initialise pokitto ..

    cookie.begin("LoadRunn", sizeof(cookie), (char*)&cookie);

    PC::begin();
    PD::loadRGBPalette(palettePico);
    PC::setFrameRate(40);
    PD::persistence = true;
    PD::invisiblecolor = 0;
    //PS::ampEnable(true);
    
    game.setup(&cookie);
    PD::setFont(font5x7);

    while (PC::isRunning()) {
    
        if (!PC::update()) continue;
        game.loop();
        
        
        PS::playTone(0, 23000, 12, 2000);
       //PS::playSFX(Sounds::sfx_3_jump, Sounds::sfx_3_jump_length);
    }
    
    return 0;

}
