#include <SDL3/SDL.h>
#include "simulation.h"
#include "simulationErrors.h"
#include "map.h"
#include "viewport.h"

Simulation::Simulation(const char* appName, const char* creatorName) : m_map(), m_viewport() {
    if (!SDL_SetAppMetadata(appName, nullptr, nullptr)) {
        throw SimulationError(SDL_GetError());
    }

    if (!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, appName)) {
        throw SimulationError(SDL_GetError());
    }

    if (!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, creatorName)) {
        throw SimulationError(SDL_GetError());
    }

    if (!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "application")) {
        throw SimulationError(SDL_GetError());
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw SimulationError(SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(appName, screenWidth, screenHeight, 0, &m_window, &m_renderer)) {
        throw SimulationError(SDL_GetError());
    }

    m_map.setTexture(m_renderer);

    m_viewport.setSize(m_map, screenWidth, screenHeight);
    m_viewport.setCoordinates(m_map, m_map.getWidth() / 2.0f, m_map.getHeight() / 2.0f);
}

Simulation::~Simulation() {
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

        // Events handling
        handleEvents(event, running);

        if (!running) {
            break;
        }

        // Movement handling
        const bool *keys{SDL_GetKeyboardState(nullptr)};
        handleMovements(keys, deltaTime);
        
        // Rendering
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
}

void Simulation::render() {
    SDL_RenderClear(m_renderer);

    m_map.render(m_renderer, m_viewport.getViewport());

    SDL_RenderPresent(m_renderer);
}
