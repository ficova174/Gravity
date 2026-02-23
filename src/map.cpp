#include <SDL3/SDL.h>

#include "map.h"
#include "mapErrors.h"

void Map::setTexture(SDL_Renderer* renderer) {
    m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_nbColumns * m_sizeSquare, m_nbRows * m_sizeSquare);
    
    if (!m_texture) {
        throw MapError("Map texture creation failed: ", SDL_GetError());
    }

    if(!SDL_SetRenderTarget(renderer, m_texture)) {
        throw MapError("Setting the render target for map texture creation failed: ", SDL_GetError());
    }

    int lr{164}, lg{176}, lb{190};  // light
    int dr{87}, dg{96}, db{111};    // dark

    // Walls
    if (!SDL_SetRenderDrawColor(renderer, dr, dg, db, 255)) {
        throw MapError("Setting the render draw color 1 for map texture creation failed: ", SDL_GetError());
    }

    SDL_FRect rect{0.0f, 0.0f, static_cast<float>(m_nbColumns * m_sizeSquare), static_cast<float>(m_nbRows * m_sizeSquare)};

    if (!SDL_RenderFillRect(renderer, &rect)) {
        throw MapError("Filling the whole map for map texture creation failed: ", SDL_GetError());
    }

    // Interior map
    if (!SDL_SetRenderDrawColor(renderer, lr, lg, lb, 255)) {
        throw MapError("Setting the render draw color 1 for map texture creation failed: ", SDL_GetError());
    }

    rect = {static_cast<float>(m_borderSize),
            static_cast<float>(m_borderSize),
            static_cast<float>(m_nbColumns * m_sizeSquare - 2 * m_borderSize),
            static_cast<float>(m_nbRows * m_sizeSquare - 2 * m_borderSize)
    };

    if (!SDL_RenderFillRect(renderer, &rect)) {
        throw MapError("Filling the whole map for map texture creation failed: ", SDL_GetError());
    }

    if (!SDL_SetRenderDrawColor(renderer, dr, dg, db, 255)) {
        throw MapError("Setting the render draw color 2 for map texture creation failed: ", SDL_GetError());
    }

    // Drawing horizontal separators on the map
    for (int y = m_borderSize; y <= m_nbRows * m_sizeSquare - m_borderSize; y += m_sizeSquare) {
        int sizeSeparator{m_sizeSeparator};

        if (y % (5 * m_sizeSquare) == 0) {
            sizeSeparator *= 2;
        }

        rect = {static_cast<float>(m_borderSize),
                static_cast<float>(y - sizeSeparator / 2),
                static_cast<float>(m_nbColumns * m_sizeSquare - m_borderSize),
                static_cast<float>(sizeSeparator)
        };

        if (!SDL_RenderFillRect(renderer, &rect)) {
            throw MapError("Rendering a separator for map texture creation failed: ", SDL_GetError());
        }
    }
    
    // Drawing vertical separators on the map
    for (int x = m_borderSize; x <= m_nbColumns * m_sizeSquare - m_borderSize; x += m_sizeSquare) {
        int sizeSeparator{m_sizeSeparator};

        if (x % (5 * m_sizeSquare) == 0) {
            sizeSeparator *= 2;
        }

        rect = {static_cast<float>(x - sizeSeparator / 2), 
                static_cast<float>(sizeSeparator),
                static_cast<float>(sizeSeparator),
                static_cast<float>(m_nbRows * m_sizeSquare - m_borderSize)
        };

        if (!SDL_RenderFillRect(renderer, &rect)) {
            throw MapError("Rendering a separator for map texture creation failed: ", SDL_GetError());
        }
    }

    if (!SDL_SetRenderTarget(renderer, nullptr)) {
        throw MapError("Setting the render target back to the screen during map creation failed: ", SDL_GetError());
    }

    m_viewportRect = {0.0f,
                      0.0f,
                      static_cast<float>(m_nbColumns * m_sizeSquare),
                      static_cast<float>(m_nbRows * m_sizeSquare)
    };
    m_particlesRect = {static_cast<float>(m_borderSize),
                       static_cast<float>(m_borderSize),
                       static_cast<float>(m_nbColumns * m_sizeSquare - 2 * m_borderSize),
                       static_cast<float>(m_nbRows * m_sizeSquare - 2 * m_borderSize)
    };
}

void Map::render(SDL_Renderer *renderer, const SDL_FRect gameViewport) {
    if (!SDL_RenderTexture(renderer, m_texture, &gameViewport, nullptr)) {
        throw MapError("Rendering the map texture failed: ", SDL_GetError());
    }
}
