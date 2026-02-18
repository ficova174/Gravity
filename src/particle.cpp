#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "particle.h"
#include "particleErrors.h"
#include "map.h"

Particle::Particle(float mass) : m_mass(mass) {
    if (mass < 1.0f) {
        throw std::invalid_argument("Mass cannot be less than 1 kg nor negative");
    }

    tempSurface = SDL_CreateSurface(sharedParticleDiameter, sharedParticleDiameter, SDL_PIXELFORMAT_RGBA8888);

    if (!tempSurface) {
        throw ParticleError("Standard particle pixel surface creation failed: ", SDL_GetError());
    }

    if (!SDL_LockSurface(tempSurface)) {
        throw ParticleError("Locking up standard particle pixel surface failed: ", SDL_GetError());
    }

    Uint8 r{0}, g{0}, b{255}, a{200};

    int radius{sharedParticleDiameter / 2};
    int dx, dy;

    for (int y = 0; y < sharedParticleDiameter; ++y) {
        for (int x = 0; x < sharedParticleDiameter; ++x) {
            dx = x - radius;
            dy = y - radius;

            if (dx * dx + dy * dy <= radius * radius) {
                if (!SDL_WriteSurfacePixel(tempSurface, x, y, r, g, b, a)) {
                    throw ParticleError("Writing pixel on standard particle pixel surface failed: ", SDL_GetError());
                }
            }
        }
    }

    SDL_UnlockSurface(tempSurface);
}

void Particle::setSharedTexture(SDL_Renderer* renderer) {
    m_texture = SDL_CreateTextureFromSurface(renderer, tempSurface);

    if (!m_texture) {
        throw ParticleError("Creating Texture from standard particle of pixel surface failed: ", SDL_GetError());
    }

    SDL_DestroySurface(tempSurface);

    if (!SDL_GetTextureSize(m_texture, &sharedParticleWidth, &sharedParticleHeight)) {
        throw ParticleError("Getting shared particle texture size failed: ", SDL_GetError());
    }

    if (sharedParticleMass < 1.0f) {
        throw std::domain_error("When computing particleDiameter we divide by sharedParticleMass and take the square root, so we want to avoid unstability and domain error");
    }

    float particleDiameter = static_cast<float>(sharedParticleDiameter) * std::sqrt(m_mass / sharedParticleMass);

    if (particleDiameter < 5.0f) {
        throw std::domain_error("The ParticleDiameter that has been computed is too small to be displayed on screen");
    }

    m_particle.w = particleDiameter;
    m_particle.h = particleDiameter;
}

void Particle::setCoordinates(const Map &map, float x, float y) {
    m_particle.x = x;
    m_particle.y = y;

    m_particle.x = std::clamp(m_particle.x, 0.0f, map.getWidth() - m_particle.w);
    m_particle.y = std::clamp(m_particle.y, 0.0f, map.getHeight() - m_particle.h);
}

void Particle::move(const Map& map, const float deltaTime) {
    checkWallCollision(map);

    m_particle.x += m_xSpeed * deltaTime;
    m_particle.y += m_ySpeed * deltaTime;
}

void Particle::checkWallCollision(const Map& map) {
    const float maxX = map.getWidth() - m_particle.w;
    const float maxY = map.getHeight() - m_particle.h;

    if (m_particle.x < 0.0f) {
        m_xSpeed = -m_xSpeed;
        m_particle.x = 0.0f;
    }
    else if (m_particle.x > maxX) {
        m_xSpeed = -m_xSpeed;
        m_particle.x = maxX;
    }

    if (m_particle.y < 0.0f) {
        m_ySpeed = -m_ySpeed;
        m_particle.y = 0.0f;
    }
    else if (m_particle.y > maxY) {
        m_ySpeed = -m_ySpeed;
        m_particle.y = maxY;
    }
}

void Particle::render(SDL_Renderer* renderer, SDL_FRect simulationViewport, const float screenWidth) {
    float scale = screenWidth / simulationViewport.w;

    m_viewport.x = (m_particle.x - simulationViewport.x) * scale;
    m_viewport.y = (m_particle.y - simulationViewport.y) * scale;
    m_viewport.w = m_particle.w * scale;
    m_viewport.h = m_particle.h * scale;

    bool conditionX{m_particle.x + m_particle.w < simulationViewport.x || m_particle.x > simulationViewport.x + simulationViewport.w};
    bool conditionY{m_particle.y + m_particle.h < simulationViewport.y || m_particle.y > simulationViewport.y + simulationViewport.h};

    if ((!conditionX) && (!conditionY)) {
        if (!SDL_RenderTexture(renderer, m_texture, nullptr, &m_viewport)) {
            throw ParticleError("Rendering the particle failed: ", SDL_GetError());
        }
    }
}
