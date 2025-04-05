#include <iostream>

#include "include/Game.h"

int main(int argc, char* args[]) {
    Game game;
    
    if (!game.init()) {
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }
    
    game.run();
    
    return 0;
}