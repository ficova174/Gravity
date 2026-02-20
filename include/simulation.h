#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "map.h"
#include "viewport.h"
#include "particle.h"

/**
 * @class Simulation
 * @brief Manages the particle simulation, including spawning, updates, and rendering.
 * @details The Simulation class handles initialization of particles, user input events,
 *          rendering, viewport management, and the game loop.
 * @author Axel LT
 * @since 2026-02-17
 */
class Simulation {
public:
    /**
     * @brief Construct a Simulation object
     * @param appName Name of the application
     * @param creatorName Name of the creator
     * @param initialNBParticles Number of initial particles to spawn
     */
    Simulation(const char* appName, const char* creatorName, int initialNBParticles);

    /**
     * @brief Destroy the Simulation object and clean up resources
     */
    ~Simulation();


    /**
     * @brief Spawn the initial particles on the map
     * @details Particles are spawned randomly without overlapping.
     *          Generate a random mass
     *          Generate the Particle
     *          Generate random coordinates on the map (minus the sides, after genererating the particle we know its radius)
     *          Check for collision with other particles (naive approach)
     *          If collision then generate other coordinates until it works
     * @warning The map needs to have enough blank space!
     * @param nbParticles Number of particles to spawn
     */
    void spawnParticles(int nbParticles);

    /**
     * @brief Run the main simulation loop
     * @details Handles events, updates particle positions, and renders the scene until exit.
     */
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
    static constexpr float screenHeight{800.0f};
    static constexpr float screenWidth{1200.0f};
    static constexpr float screenRatio{screenWidth/screenHeight};
};
