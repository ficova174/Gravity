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
     */
    Simulation(const char* appName, const char* creatorName);

    /**
     * @brief Destroy the Simulation object and clean up resources
     */
    ~Simulation();


    /**
     * @brief Spawn or destroy particles onto the map
     * @details Particles are spawned randomly without overlapping
     *          They are destroyed if the nbParticlesWanted is lower than the actual number of particles
     *          Generate a random mass
     *          Generate the Particle
     *          Generate random coordinates on the map (minus the sides, after genererating the particle we know its radius)
     *          Check for collision with other particles (naive approach)
     *          If collision then generate other coordinates until it works
     * @warning The map needs to have enough blank space!
     * @param nbParticlesWanted Number of particles we want
     */
    void spawnDestroyParticles(int nbParticlesWanted);

    /**
     * @brief Run the main simulation loop
     * @details Handles events, updates particle positions, and renders the scene until exit.
     */
    void run();

private:
    float getTotalKineticEnergy();

    void destroyParticles(int nbParticles);
    void spawnParticles(int nbParticles);
    void myImGuiWindow();
    void handleEvents(SDL_Event &event, bool &running);
    void handleZoom(SDL_Event &event);
    void handleMovements(const bool *keys, float deltaTime);
    void render();


    SDL_Window* m_window{nullptr};
    SDL_Renderer* m_renderer{nullptr};

    Map m_map;
    Viewport m_viewport;
    std::vector<Particle> m_particles;

    int nbParticlesSim{0};
    int nbParticlesWantedSim{3};
    static constexpr int maxNBParticlesSim{1000};

    static constexpr float targetFPS{120.0f};
    static constexpr float screenHeight{800.0f};
    static constexpr float screenWidth{1200.0f};
    static constexpr float screenRatio{screenWidth/screenHeight};
};
