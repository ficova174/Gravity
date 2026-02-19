#include <SDL3/SDL.h>
#include "map.h"
#include "mapErrors.h"

void Map::setTexture(SDL_Renderer* renderer) {
    SDL_Surface* scaledSurface{setSurface()};
    m_texture = SDL_CreateTextureFromSurface(renderer, scaledSurface);

    if (!m_texture) {
        SDL_DestroySurface(scaledSurface);
        throw MapError("Creating Texture from map of scaled pixel surface failed: ", SDL_GetError());
    }

    SDL_DestroySurface(scaledSurface);

    if (!SDL_GetTextureSize(m_texture, &m_w, &m_h)) {
        throw MapError("Getting the map texture size failed: ", SDL_GetError());
    }
}

SDL_Surface* Map::setSurface() {
    constexpr int nbRows{50};
    constexpr int nbColumns{100};
    constexpr int size{50};

    SDL_Surface* surface{SDL_CreateSurface(nbColumns, nbRows, SDL_PIXELFORMAT_RGBA8888)};

    if (!surface) {
        throw MapError("Map pixel surface creation failed: ", SDL_GetError());
    }

    if (!SDL_LockSurface(surface)) {
        SDL_DestroySurface(surface);
        throw MapError("Locking up map pixel surface failed: ", SDL_GetError());
    }

    Uint8 r1{50}, g1{50}, b1{50}, a1{50};
    Uint8 r2{150}, g2{150}, b2{150}, a2{150};

    bool isEvenRow, isEvenColumn;
    for (int i = 0; i < nbRows; ++i) {
        for (int j = 0; j < nbColumns; ++j) {
            isEvenRow = ((i & 1) == 0);
            isEvenColumn = ((j & 1) == 0);

            if ((isEvenRow && isEvenColumn) || (!isEvenRow && !isEvenColumn)) {
                if (!SDL_WriteSurfacePixel(surface, j, i, r1, g1, b1, a1)) {
                    SDL_DestroySurface(surface);
                    throw MapError("Writing pixel on map pixel surface case 1 failed: ", SDL_GetError());
                }
            }
            else {
                if (!SDL_WriteSurfacePixel(surface, j, i, r2, g2, b2, a2)) {
                    SDL_DestroySurface(surface);
                    throw MapError("Writing pixel on map pixel surface case 2 failed: ", SDL_GetError());
                }
            }
        }
    }

    SDL_UnlockSurface(surface);

    SDL_Surface* scaledSurface = SDL_ScaleSurface(surface, nbColumns*size, nbRows*size, SDL_SCALEMODE_PIXELART);

    if (!scaledSurface) {
        SDL_DestroySurface(surface);
        throw MapError("Scaling the map pixel surface failed: ", SDL_GetError());
    }

    SDL_DestroySurface(surface);

    return scaledSurface;
}

void Map::render(SDL_Renderer *renderer, SDL_FRect gameViewport) {
    if (!SDL_RenderTexture(renderer, m_texture, &gameViewport, nullptr)) {
        throw MapError("Rendering the map texture failed: ", SDL_GetError());
    }
}
