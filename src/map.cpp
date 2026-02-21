#include <SDL3/SDL.h>

#include "map.h"
#include "mapErrors.h"

void Map::setTexture(SDL_Renderer* renderer) {
    m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, static_cast<int>(m_nbColumns * m_size), static_cast<int>(m_nbRows * m_size));
    
    if (!m_texture) {
        throw MapError("Map texture creation failed: ", SDL_GetError());
    }

    if(!SDL_SetRenderTarget(renderer, m_texture)) {
        throw MapError("Setting the render target for map texture creation failed: ", SDL_GetError());
    }

    for (int col = 0; col < m_nbColumns; ++col) {
        for (int row = 0; row < m_nbRows; ++row) {
            bool isColored{(col + row) % 2 == 0};
            int rgba{isColored ? 50 : 150};
            
            if (!SDL_SetRenderDrawColor(renderer, rgba, rgba, rgba, 255)) {
                throw MapError("Setting the render draw color for map texture creation failed: ", SDL_GetError());
            }

            const SDL_FRect rect{col * m_size, row * m_size, m_size, m_size};

            if (!SDL_RenderFillRect(renderer, &rect)) {
                throw MapError("Filling a square for map texture creation failed: ", SDL_GetError());
            }
        }
    }

    if (!SDL_SetRenderTarget(renderer, nullptr)) {
        throw MapError("Setting the render target back to the screen during map creation failed: ", SDL_GetError());
    }
}

void Map::render(SDL_Renderer *renderer, const SDL_FRect gameViewport) {
    if (!SDL_RenderTexture(renderer, m_texture, &gameViewport, nullptr)) {
        throw MapError("Rendering the map texture failed: ", SDL_GetError());
    }
}
