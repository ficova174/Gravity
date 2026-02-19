#pragma once

#include <SDL3/SDL.h>

/**
 * @class Map
 * @brief Represents the simulation map
 * @details The map is made by "hand" using a surface and writing pixels.
 * @author Axel LT
 * @since 2026-02-17
 */
class Map {
public:
    ~Map() {SDL_DestroyTexture(m_texture);}

    /**
     * @brief Get the width of the map
     * @return Map width in pixels
     */
    float getWidth() const {return m_w;}

    /**
     * @brief Get the height of the map
     * @return Map height in pixels
     */
    float getHeight() const {return m_h;}

    /**
     * @brief Load and set the texture for the map
     * @param renderer SDL_Renderer to render to
     */
    void setTexture(SDL_Renderer* renderer);


    /**
     * @brief Render the map according to the current viewport
     * @param renderer SDL_Renderer to render to
     * @param gameViewport Portion of the map currently visible on screen
     */
    void render(SDL_Renderer* renderer, const SDL_FRect gameViewport);

private:
    /**
     * @brief Create a surface representing the map that will be loaded as a texture
     * @see Map::setTexture
     */
    SDL_Surface* setSurface();

    /// SDL texture representing the map
    SDL_Texture* m_texture{nullptr};

    /// Map dimensions in pixels
    float m_w;
    float m_h;
};