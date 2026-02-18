#pragma once

#include <SDL3/SDL.h>
#include "map.h"

class Particle {
public:
    static void setSharedTexture(SDL_Renderer* renderer);
    static void destroySharedTexture() {SDL_DestroyTexture(m_texture);}

    Particle(float mass, float xSpeed, float ySpeed);

    void setCoordinates(const Map &map, float x, float y);

    void move(const Map& map, const float deltaTime);
    void render(SDL_Renderer* renderer, SDL_FRect simulationViewport, const float screenWidth);

private:
    static SDL_Surface* setSharedSurface();

    inline static SDL_Texture* m_texture{nullptr};
    inline static float sharedParticleWidth, sharedParticleHeight;
    // We arbitrarily chose a mass of 100kg and a size of 1001 pixels (odd for symmetry)
    inline static constexpr float sharedParticleMass{100.0f};
    inline static constexpr int sharedParticleDiameter{1001};

    void checkWallCollision(const Map& map);

    SDL_FRect m_particle{0.0f, 0.0f, 0.0f, 0.0f};
    SDL_FRect m_viewport{0.0f, 0.0f, 0.0f, 0.0f};
    const float m_mass;
    float m_xSpeed;
    float m_ySpeed;
};
