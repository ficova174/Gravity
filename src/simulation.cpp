#include <SDL3/SDL.h>
#include <vector>
#include <random>
#include "simulation.h"
#include "simulationErrors.h"
#include "map.h"
#include "viewport.h"
#include "particle.h"

Simulation::Simulation(const char* appName, const char* creatorName, int initialNBParticles) : m_map(), m_viewport() {
    if (!SDL_SetAppMetadata(appName, nullptr, nullptr)) {
        throw SimulationError("Setting up the app metadata failed: ", SDL_GetError());
    }

    if (!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, creatorName)) {
        throw SimulationError("Setting up the creator name for the project failed: ", SDL_GetError());
    }

    if (!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "application")) {
        throw SimulationError("Setting up the application metadata type failed: ", SDL_GetError());
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw SimulationError("Video initialisation failed: ", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(appName, screenWidth, screenHeight, 0, &m_window, &m_renderer)) {
        throw SimulationError("Creation of the window and renderer failed: ", SDL_GetError());
    }

    m_map.setTexture(m_renderer);
    Particle::setSharedTexture(m_renderer);

    m_viewport.setSize(m_map, screenWidth, screenHeight);
    m_viewport.setCoordinates(m_map, m_map.getWidth() / 2.0f, m_map.getHeight() / 2.0f);

    m_particles.reserve(initialNBParticles);

    spawnParticles(initialNBParticles);
}

void Simulation::spawnParticles(int nbParticles) {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto randomMass = [&gen]() -> int {
        int minMass{static_cast<int>(0.01f * Particle::sharedParticleMass)};
        int maxMass{static_cast<int>(0.1f * Particle::sharedParticleMass)};
        std::uniform_int_distribution<int> distMass(minMass, maxMass);
        return distMass(gen);
    };

    auto randomVelocity = [&gen]() -> std::pair<float, float> {
        float minSpeed{100.0f};
        float maxSpeed{500.0f};
        std::uniform_real_distribution<float> distSpeed(minSpeed, maxSpeed);
        std::uniform_int_distribution<int> distSign(0, 1);
        auto randomSign = [&gen, &distSign]() -> float {return distSign(gen) == 0 ? -1.0f : 1.0f;};
        return {randomSign() * distSpeed(gen), randomSign() * distSpeed(gen)};
    };

    auto randomCoordinate = [&gen](float min, float max) -> float {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    };

    for (int i = 0; i < nbParticles; ++i) {
        int mass{randomMass()};
        auto [xSpeed, ySpeed] = randomVelocity();

        m_particles.emplace_back(mass, xSpeed, ySpeed);
        Particle& particle = m_particles.back();

        bool collision{false};
        do {
            float minX{particle.getParticle().w};
            float maxX{m_map.getWidth() - particle.getParticle().w};
            float x{randomCoordinate(minX, maxX)};

            float minY{particle.getParticle().h};
            float maxY{m_map.getHeight() - particle.getParticle().h};
            float y{randomCoordinate(minY, maxY)};

            particle.setCoordinates(m_map, x, y);

            if (m_particles.size() == 1) {
                collision = false;
                break;
            }

            // We need to put the collision back to false!
            collision = false;
            for (Particle& otherParticle : m_particles) {
                if (&particle == &otherParticle) {
                    continue;
                }

                if (particle.checkCollisionParticle(otherParticle)) {
                    collision = true;
                    break;
                }
            }
        }
        while (collision);
    }
}

Simulation::~Simulation() {
    Particle::destroySharedTexture();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Simulation::run() {
    Uint64 perfFreq{SDL_GetPerformanceFrequency()};
    Uint64 lastCounter{SDL_GetPerformanceCounter()};

    SDL_Event event;
    bool running{true};

    while (running) {
        // FPS counter
        Uint64 currentCounter{SDL_GetPerformanceCounter()};
        // Convert to seconds
        float deltaTime{static_cast<float>(currentCounter - lastCounter) / static_cast<float>(perfFreq)};
        lastCounter = currentCounter;

        handleEvents(event, running);

        if (!running) {
            break;
        }

        const bool *keys{SDL_GetKeyboardState(nullptr)};
        handleMovements(keys, deltaTime);
        
        render();

        float targetFrameTime {1.0f / targetFPS};
        float frameTime {static_cast<float>(SDL_GetPerformanceCounter() - currentCounter) / perfFreq};

        if (frameTime < targetFrameTime) {
            SDL_Delay(static_cast<Uint32>((targetFrameTime - frameTime) * 1.0e3f));
        }
    }
}

void Simulation::handleEvents(SDL_Event &event, bool &running) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                return;
            case SDL_EVENT_MOUSE_WHEEL:
                handleZoom(event);
                break;
        }
    }
}

void Simulation::handleZoom(SDL_Event &event) {
    float viewportChangeX{event.wheel.y * m_viewport.getZoomSpeed()};
    float viewportChangeY{viewportChangeX / aspectRatio};

    m_viewport.zoom(m_map, viewportChangeX, viewportChangeY);
}

void Simulation::handleMovements(const bool *keys, float deltaTime) {
    SDL_PumpEvents();

    m_viewport.move(m_map, keys, deltaTime);

    for (Particle& particle : m_particles) {
        particle.move(m_map, deltaTime);
    }
}

void Simulation::render() {
    SDL_RenderClear(m_renderer);

    m_map.render(m_renderer, m_viewport.getViewport());

    for (Particle& particle : m_particles) {
        particle.render(m_renderer, m_viewport.getViewport(), screenWidth);
    }

    SDL_RenderPresent(m_renderer);
}
