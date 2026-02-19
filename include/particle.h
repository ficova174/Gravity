#pragma once

#include <SDL3/SDL.h>
#include "map.h"

/**
 * @class Particle
 * @brief Represents a single particle in the simulation
 * @details Handles particle creation, movement, collisions, rendering, and shared texture management.
 * @warning The texture is shared between instances thus we can't delete the texture until the end of the simulation
 * @author Axel LT
 * @version 1.0.0
 * @since 2026-02-18
 */
class Particle {
public:
    /**
     * @brief Set the shared texture for all particles
     * @param renderer SDL_Renderer to render to
     */
    static void setSharedTexture(SDL_Renderer* renderer);

    /**
     * @brief Destroy the shared particle texture
     */
    static void destroySharedTexture() {SDL_DestroyTexture(m_texture);}

    /// Shared constants
    inline static constexpr int sharedParticleMass{100};      ///< Arbitrary mass for the reference particle
    inline static constexpr int sharedParticleDiameter{1001}; ///< Odd size of the reference particle for symmetry


    /**
     * @brief Construct a particle with mass and initial velocity
     * @param mass Particle mass
     * @param xSpeed Initial velocity in x direction
     * @param ySpeed Initial velocity in y direction
     */
    Particle(int mass, float xSpeed, float ySpeed);


    /**
     * @brief Set particle coordinates on the map
     * @param map Reference to the simulation map
     * @param x X coordinate
     * @param y Y coordinate
     */
    void setCoordinates(const Map &map, float x, float y);

    /**
     * @brief Get the SDL floating rectangle representing particle position and size
     * @return SDL_FRect containing particle coordinates and dimensions
     */
    SDL_FRect getParticle() const {return m_particle;}


    /**
     * @brief Move the particle based on its velocity and deltaTime
     * @param map Reference to the map (for collision checking)
     * @param deltaTime Time elapsed since last frame
     */
    void move(const Map& map, const float deltaTime);

    /**
     * @brief Check collision with another particle
     * @note Member function needs to be public as it's used during the particle's creation
     * @param otherParticle References particle to check for collision
     * @return True if collision else False
     * @see Simulation::spawnParticles
     */
    bool checkCollisionParticle(Particle& otherParticle);

    /**
     * @brief Render the particle on the screen
     * @details Only render the particle if it is in the viewport.
     * @param renderer SDL_Renderer to render to
     * @param simulationViewport The current simulation viewport
     * @param screenWidth Width of the screen for scaling
     */
    void render(SDL_Renderer* renderer, SDL_FRect simulationViewport, const float screenWidth);

private:
    /**
     * @brief Create and return the shared SDL surface for all particles
     * @see Particle::setSharedTexture
     * @return Pointer to SDL_Surface
     */
    static SDL_Surface* setSharedSurface();

    /// Shared texture for all particles
    inline static SDL_Texture* m_texture{nullptr};


    /**
     * @brief Check for wall collisions and adjust particle position/velocity
     * @details Clamp the position on the map and reverse the normal component of velocity
     * @see Particle::move
     * @param map Reference to the map
     */
    void checkWallCollision(const Map& map);

    /**
     * @brief In the case of a collision we are computing its outcome
     * @param otherParticle References particle we collided with
     * @see Particle::checkCollisionParticle
     */
    void solveCollisionParticle(Particle& otherParticle);


    /// Particle position and size
    SDL_FRect m_particle{0.0f, 0.0f, 0.0f, 0.0f};

    /**
     * @brief Particle center position
     * @warning This should be computed only when needed!
     */
    SDL_FPoint m_center{0.0f, 0.0f};

    /// Particle viewport for rendering
    SDL_FRect m_viewport{0.0f, 0.0f, 0.0f, 0.0f};

    /// Particle mass (constant per instance)
    const int m_mass;

    /// Particle velocity components
    float m_xSpeed;
    float m_ySpeed;
};
