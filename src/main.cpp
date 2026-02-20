#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <exception>
#include "version.h"

#include "simulation.h"

int main() {
    try {
        Simulation simulation("Gravity Simulation", "Axel LT");
        simulation.run();
    }
    catch (const std::exception& e) {
        SDL_Log(e.what());
        return -1;
    }

    return 0;
}
