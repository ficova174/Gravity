#pragma once

#include <SDL3/SDL.h>
#include "Eigen/Dense"

#include "map.h"

/**
 * @class Particle
 * @brief Represents a single particle in the simulation
 * @details Handles particle creation, movement, collisions, rendering, and shared texture management.
 * @warning The texture is shared between instances thus we can't delete the texture until the end of the simulation
 * @author Axel LT
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
     * @param velocity Initial velocity vector
     */
    Particle(const int mass, const Eigen::Vector2f velocity);


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
     * @brief Check collision with another particle during particle instanciation
     * @warning Can't be used before solveCollisionParticle as it uses the squared distance
     * @param otherParticle References particle to check for collision
     * @return True if collision else False
     * @see Simulation::spawnParticles
     */
    bool checkCollisionInit(const Particle& otherParticle);

    /**
     * @brief Move the particle based on its velocity and deltaTime
     * @details Wall and particle collisions are checked AFTER moving
     * @param deltaTime Time elapsed since last frame
     */
    void move(const float deltaTime);

    /**
     * @brief Solve wall collisions and adjust particle position/velocity
     * @details Clamp the position on the map and reverse the normal component of velocity
     * @note Sould be used after moving the particle
     * @param map Reference to the map
     */
    void solveWallCollision(const Map& map);

    /**
     * @brief Check collision with another particle and compute the outcome
     * @param otherParticle References particle we could collide with
     */
    void checkSolveCollision(Particle& otherParticle);

    /**
     * @brief Render the particle on the screen
     * @details Only render the particle if it is in the viewport.
     * @param renderer SDL_Renderer to render to
     * @param simulationViewport The current simulation viewport
     * @param screenWidth Width of the screen for scaling
     */
    void render(SDL_Renderer* renderer, const SDL_FRect simulationViewport, const float screenWidth);

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
     * @brief Compute the velocity components in world coordinates
     * @param otherParticle References particle we collided with
     * @param normalUnitVector Normal unit vector of the collision plane
     * @see Particle::checkSolveCollision
     */
    void solveCollision(Particle& otherParticle, const Eigen::Vector2f& normalUnitVector);


    /// Particle position and size
    SDL_FRect m_particle{0.0f, 0.0f, 0.0f, 0.0f};

    /// Particle viewport for rendering
    SDL_FRect m_viewport{0.0f, 0.0f, 0.0f, 0.0f};

    /// Particle mass (constant per instance)
    const int m_mass;

    /// Particle velocity components (x, y)
    Eigen::Vector2f m_velocity;
};
