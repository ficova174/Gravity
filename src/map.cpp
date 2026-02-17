#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "map.h"
#include "mapErrors.h"

Map::~Map() {
    SDL_DestroyTexture(m_texture);
}

void Map::setTexture(SDL_Renderer *renderer) {
    m_texture = IMG_LoadTexture(renderer, "../assets/map.png");

    if (!m_texture) {
        throw MapError("Map texture did not load correctly");
    }

    if (!SDL_GetTextureSize(m_texture, &w, &h)) {
        throw MapError(SDL_GetError());
    }
}

void Map::render(SDL_Renderer *renderer, SDL_FRect gameViewport) {
    if (!SDL_RenderTexture(renderer, m_texture, &gameViewport, nullptr)) {
        throw MapError(SDL_GetError());
    }
}
