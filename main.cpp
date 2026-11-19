#include "game.h"
#include <cstdio>

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init()) {
        fprintf(stderr, "Failed to initialize game.\n");
        return 1;
    }
    game.run();
    return 0;
}
