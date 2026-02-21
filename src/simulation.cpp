#include <SDL3/SDL.h>
#include <vector>
#include <random>
#include <cmath>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "simulation.h"
#include "simulationErrors.h"
#include "map.h"
#include "viewport.h"
#include "particle.h"

Simulation::Simulation(const char* appName, const char* creatorName) : m_map(300, 300, 50), m_viewport() {
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

    m_particles.reserve(maxNBParticlesSim);
    spawnDestroyParticles(nbParticlesWantedSim);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void Simulation::spawnDestroyParticles(int nbParticlesWanted) {
    if (nbParticlesWanted < 0) {
        throw SimulationError("You can't have a negative number of particles");
    }

    int diff{nbParticlesSim - nbParticlesWanted};

    if (diff == 0) {
        return;
    } else if (diff > 0) {
        destroyParticles(diff);
    } else {
        int absDiff{-diff};
        spawnParticles(absDiff);
    }

    nbParticlesSim = nbParticlesWanted;
}

void Simulation::destroyParticles(int nbParticles) {
    for (int i = 0; i < nbParticles; ++i) {
        // It calls the object
        m_particles.pop_back();
    }
}

void Simulation::spawnParticles(int nbParticles) {
    std::random_device rd;
    std::mt19937 gen(rd());

    int minMass{static_cast<int>(0.01f * Particle::sharedParticleMass)};
    int maxMass{static_cast<int>(0.1f * Particle::sharedParticleMass)};
    std::uniform_int_distribution<int> distMass(minMass, maxMass);

    float maxSpeed{5000.0f};
    std::uniform_real_distribution<float> distSpeed(0.0f, maxSpeed);
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * M_PI);

    auto randomCoordinate = [&gen](float min, float max) -> float {
        std::uniform_real_distribution<float> distCoord(min, max);
        return distCoord(gen);
    };

    for (int i = 0; i < nbParticles; ++i) {
        int mass{distMass(gen)};
        Eigen::Vector2f velocity{distSpeed(gen) * std::cos(distAngle(gen)),
                                 distSpeed(gen) * std::sin(distAngle(gen))};

        m_particles.emplace_back(mass, velocity);
        Particle& particle{m_particles.back()};

        bool collision{false};
        int counter{0};
        do {
            // We want the particle to be in the map
            // We know the particle's radius
            
            ++counter;
            constexpr int nbTimesToTry{3};
            if (counter > nbTimesToTry * nbParticles) {
                throw SimulationError("When initialising there is not enough place to put the particles onto the map");
            }

            float minX{particle.getParticle().w};
            float maxX{m_map.getWidth() - particle.getParticle().w};

            if (minX >= maxX) {
                throw SimulationError("Map is too small so the particle don't fit in it");
            }

            float x{randomCoordinate(minX, maxX)};

            float minY{particle.getParticle().h};
            float maxY{m_map.getHeight() - particle.getParticle().h};

            if (minY >= maxY) {
                throw SimulationError("Map is too small so the particle don't fit in it");
            }

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

                if (particle.checkCollisionInit(otherParticle)) {
                    collision = true;
                    break;
                }
            }
        }
        while (collision);
    }
}

Simulation::~Simulation() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

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
        float deltaTime{static_cast<float>(currentCounter - lastCounter) / static_cast<float>(perfFreq)};  // Convert to seconds
        lastCounter = currentCounter;

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        myImGuiWindow();

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
        ImGui_ImplSDL3_ProcessEvent(&event);

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
    float viewportChangeY{viewportChangeX / screenRatio};

    m_viewport.zoom(m_map, viewportChangeX, viewportChangeY);
}

void Simulation::handleMovements(const bool *keys, float deltaTime) {
    SDL_PumpEvents();
    m_viewport.move(m_map, keys, deltaTime);

    for (size_t i = 0; i < m_particles.size(); ++i) {
        Particle& particle{m_particles.at(i)};

        particle.move(deltaTime);
        particle.solveWallCollision(m_map);

        for (size_t j = i + 1; j < m_particles.size(); ++j) {
            Particle& otherParticle{m_particles.at(j)};

            particle.checkSolveCollision(otherParticle);
        }
    }
}

void Simulation::render() {
    if (!SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255)) {
        SimulationError("Setting renderer draw color failed: ", SDL_GetError());
    }

    SDL_RenderClear(m_renderer);

    // Order of rendering matters for layering

    m_map.render(m_renderer, m_viewport.getViewport());

    for (Particle& particle : m_particles) {
        particle.render(m_renderer, m_viewport.getViewport(), screenWidth);
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);

    SDL_RenderPresent(m_renderer);
}

void Simulation::myImGuiWindow() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::Begin("Simulation toolbox and information center", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("Simulation average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    // Nb particles simulation
    if (ImGui::Button("-") && (nbParticlesWantedSim > 0)) {
        --nbParticlesWantedSim;
    }
    ImGui::SameLine();
    ImGui::SliderInt("##nbParticles slider", &nbParticlesWantedSim, 0, maxNBParticlesSim);
    ImGui::SameLine();
    if (ImGui::Button("+") && (nbParticlesWantedSim < maxNBParticlesSim)) {
        ++nbParticlesWantedSim;
    }
    ImGui::SameLine();
    ImGui::Text("Particles");

    spawnDestroyParticles(nbParticlesWantedSim);

    ImGui::Text("Total kinetic energy (MJ) : %.3f", getTotalKineticEnergy() / 1e6);

    ImGui::End();
}

float Simulation::getTotalKineticEnergy() {
    float sum{0.0f};

    for (Particle& particle : m_particles) {
        sum += particle.getKineticEnergy();
    }

    return sum;
}
