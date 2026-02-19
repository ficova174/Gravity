#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "Eigen/Dense"
#include "particle.h"
#include "particleErrors.h"
#include "map.h"

void Particle::setSharedTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface{setSharedSurface()};

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!m_texture) {
        SDL_DestroySurface(surface);
        throw ParticleError("Creating Texture from standard particle of pixel surface failed: ", SDL_GetError());
    }

    SDL_DestroySurface(surface);

    if (sharedParticleMass < 1) {
        throw std::domain_error("When computing particleDiameter we divide by sharedParticleMass and take the square root, so we want to avoid unstability and domain error");
    }
}

SDL_Surface* Particle::setSharedSurface() {
    SDL_Surface* surface{SDL_CreateSurface(sharedParticleDiameter, sharedParticleDiameter, SDL_PIXELFORMAT_RGBA8888)};

    if (!surface) {
        throw ParticleError("Standard particle pixel surface creation failed: ", SDL_GetError());
    }

    if (!SDL_LockSurface(surface)) {
        SDL_DestroySurface(surface);
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
                if (!SDL_WriteSurfacePixel(surface, x, y, r, g, b, a)) {
                    SDL_DestroySurface(surface);
                    throw ParticleError("Writing pixel on standard particle pixel surface failed: ", SDL_GetError());
                }
            }
        }
    }

    SDL_UnlockSurface(surface);

    return surface;
}

Particle::Particle(const int mass, const Eigen::Vector2f velocity) : m_mass(mass), m_velocity(velocity) {
    if (mass < 1) {
        throw std::invalid_argument("Mass cannot be less than 1 kg nor negative");
    }

    float particleDiameter = static_cast<float>(sharedParticleDiameter) * std::sqrt(static_cast<float>(m_mass) / static_cast<float>(sharedParticleMass));

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

void Particle::move(const float deltaTime) {
    Eigen::Vector2f deltaPos{m_velocity * deltaTime};

    m_particle.x += deltaPos(0);
    m_particle.y += deltaPos(1);
}

void Particle::solveWallCollision(const Map& map) {
    const float maxX = map.getWidth() - m_particle.w;
    const float maxY = map.getHeight() - m_particle.h;

    if (m_particle.x < 0.0f) {
        m_velocity(0) = -m_velocity(0);
        m_particle.x = 0.0f;
    }
    else if (m_particle.x > maxX) {
        m_velocity(0) = -m_velocity(0);
        m_particle.x = maxX;
    }

    if (m_particle.y < 0.0f) {
        m_velocity(1) = -m_velocity(1);
        m_particle.y = 0.0f;
    }
    else if (m_particle.y > maxY) {
        m_velocity(1) = -m_velocity(1);
        m_particle.y = maxY;
    }
}

bool Particle::checkCollisionInit(const Particle& otherParticle) {
    SDL_FRect otherParticleDescriptor{otherParticle.getParticle()};

    float particleRadius{m_particle.w / 2.0f};
    float otherParticleRadius{otherParticleDescriptor.w / 2.0f};

    Eigen::Vector2f particleCenterCoord{m_particle.x + particleRadius, m_particle.y + particleRadius};
    Eigen::Vector2f otherParticleCenterCoord{otherParticleDescriptor.x + otherParticleRadius, otherParticleDescriptor.y + otherParticleRadius};
    Eigen::Vector2f deltaPos{particleCenterCoord - otherParticleCenterCoord};

    float actualCenterDistanceSquared{deltaPos.dot(deltaPos)};
    float expectedCenterDistanceSquared{(particleRadius + otherParticleRadius) * (particleRadius + otherParticleRadius)};

    if (actualCenterDistanceSquared <= expectedCenterDistanceSquared) {
        return true;
    }

    return false;
}

void Particle::checkSolveCollision(Particle& otherParticle) {
    SDL_FRect otherParticleDescriptor{otherParticle.getParticle()};

    float particleRadius{m_particle.w / 2.0f};
    float otherParticleRadius{otherParticleDescriptor.w / 2.0f};

    Eigen::Vector2f particleCenterCoord{m_particle.x + particleRadius, m_particle.y + particleRadius};
    Eigen::Vector2f otherParticleCenterCoord{otherParticleDescriptor.x + otherParticleRadius, otherParticleDescriptor.y + otherParticleRadius};
    Eigen::Vector2f deltaPos{particleCenterCoord - otherParticleCenterCoord};

    float actualCenterDistance{deltaPos.norm()};
    float expectedCenterDistance{particleRadius + otherParticleRadius};

    if (actualCenterDistance <= expectedCenterDistance) {
        Eigen::Vector2f normalUnitVector{deltaPos(0) / actualCenterDistance, deltaPos(1) / actualCenterDistance};
        solveCollision(otherParticle, normalUnitVector);
    }
}

void Particle::solveCollision(Particle& otherParticle, const Eigen::Vector2f& normalUnitVector) {
    // The main idea is that we are solving the system of equation along the normal plane of the impact
    // the collisation is perfectly elastic, the system of equation can be found online

    // 90° counterclockwise rotation matrix (it preserves distances as det(R) = 1)
    Eigen::Matrix2f R{{0, -1},
                      {1, 0}};

    const Eigen::Vector2f tangentUnitVector{R * normalUnitVector};

    Eigen::Vector2f particleVelocityNT{m_velocity.dot(normalUnitVector),
                                       m_velocity.dot(tangentUnitVector)};

    Eigen::Vector2f otherParticleVelocityNT{otherParticle.m_velocity.dot(normalUnitVector),
                                            otherParticle.m_velocity.dot(tangentUnitVector)};

    const float denominator{static_cast<float>(m_mass + otherParticle.m_mass)};

    // particle coefficients
    const float firstCoeff{(m_mass - otherParticle.m_mass) / denominator};
    const float secondCoeff{(2 * otherParticle.m_mass) / denominator};

    // other particle coefficients
    const float thirdCoeff{(2 * m_mass) / denominator};
    const float fourthCoeff{-firstCoeff};


    Eigen::Vector2f normalVelocities{firstCoeff * particleVelocityNT(0) + secondCoeff * otherParticleVelocityNT(0),
                                     thirdCoeff * particleVelocityNT(0) + fourthCoeff * otherParticleVelocityNT(0)};

    Eigen::Vector2f particleFinalVelocityNT{particleVelocityNT(0) - normalVelocities(0),
                                            particleVelocityNT(1)};

    Eigen::Vector2f otherParticleFinalVelocityNT{otherParticleVelocityNT(0) - normalVelocities(1),
                                                 otherParticleVelocityNT(1)};

    // 2D rotation matrices are orthogonal
    Eigen::Matrix2f iR{R.transpose()};

    m_velocity = iR * particleFinalVelocityNT;
    otherParticle.m_velocity = iR * otherParticleFinalVelocityNT;
}

void Particle::render(SDL_Renderer* renderer, const SDL_FRect simulationViewport, const float screenWidth) {
    float scale {screenWidth / simulationViewport.w};

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
