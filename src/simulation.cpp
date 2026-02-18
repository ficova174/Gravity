#include <SDL3/SDL.h>
#include "simulation.h"
#include "simulationErrors.h"
#include "map.h"
#include "viewport.h"
#include "particle.h"

Simulation::Simulation(const char* appName, const char* creatorName) : m_map(), m_viewport(), m_particle(10.0f) {
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
    m_particle.setSharedTexture(m_renderer);

    m_viewport.setSize(m_map, screenWidth, screenHeight);
    m_viewport.setCoordinates(m_map, m_map.getWidth() / 2.0f, m_map.getHeight() / 2.0f);
    m_particle.setCoordinates(m_map, m_map.getWidth() / 2.0f, m_map.getHeight() / 2.0f);
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
    m_particle.move(m_map, deltaTime);
}

void Simulation::render() {
    SDL_RenderClear(m_renderer);

    m_map.render(m_renderer, m_viewport.getViewport());
    m_particle.render(m_renderer, m_viewport.getViewport(), screenWidth);

    SDL_RenderPresent(m_renderer);
}
