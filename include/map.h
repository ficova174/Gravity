#pragma once

#include <SDL3/SDL.h>

class Map {
public:
    ~Map() {SDL_DestroyTexture(m_texture);}

    float getWidth() const {return m_w;}
    float getHeight() const {return m_h;}

    void setTexture(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer, SDL_FRect gameViewport);

private:
    SDL_Texture* m_texture{nullptr};
    float m_w{0.0f};
    float m_h{0.0f};
};