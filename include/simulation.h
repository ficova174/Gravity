#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "map.h"
#include "viewport.h"
#include "particle.h"

class Simulation {
public:
    Simulation(const char* appName, const char* creatorName);
    ~Simulation();

    void run();

private:
    void handleEvents(SDL_Event &event, bool &running);
    void handleZoom(SDL_Event &event);
    void handleMovements(const bool *keys, float deltaTime);
    void render();

    SDL_Window* m_window{nullptr};
    SDL_Renderer* m_renderer{nullptr};

    Map m_map;
    Viewport m_viewport;
    std::vector<Particle> m_particles;

    static constexpr float targetFPS{120.0f};

    // Screen dimensions
    static constexpr float screenHeight{800.0f};
    static constexpr float screenWidth{1200.0f};
    static constexpr float aspectRatio{screenWidth/screenHeight};
};
