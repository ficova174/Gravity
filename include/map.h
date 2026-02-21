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
    Map(int nbColumns, int nbRows, int size) : m_nbColumns(static_cast<float>(nbColumns)),
                                               m_nbRows(static_cast<float>(nbRows)),
                                               m_size(static_cast<float>(size)) {}
    ~Map() {SDL_DestroyTexture(m_texture);}

    /**
     * @brief Get the width of the map
     * @return Map width in pixels
     */
    float getWidth() const {return m_nbColumns * m_size;}

    /**
     * @brief Get the height of the map
     * @return Map height in pixels
     */
    float getHeight() const {return m_nbRows * m_size;}

    /**
     * @brief Set the texture for the map
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
    /// SDL texture representing the map
    SDL_Texture* m_texture{nullptr};

    /// Map dimensions in pixels
    float m_nbColumns;
    float m_nbRows;

    /// Map square size in pixels
    float m_size;
};