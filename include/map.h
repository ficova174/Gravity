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
    Map(int nbColumns, int nbRows) : m_nbColumns(nbColumns), m_nbRows(nbRows) {}
    ~Map() {SDL_DestroyTexture(m_texture);}


    /**
     * @brief Get the number of columns of the map
     * @return int representing the number of columns of the map
     */
    int getNBColumns() const {return m_nbColumns;}

    /**
     * @brief Get the number of rows of the map
     * @return int representing the number of rows of the map
     */
    int getNBRows() const {return m_nbRows;}

    /**
     * @brief Get the width of the map in pixels
     * @return int representing the width of the map in pixels
     */
    int getWidth() const {return m_nbColumns * m_sizeSquare;}

    /**
     * @brief Get the rectangle used by the viewport
     * @return SDL_FRect representing the whole map
     */
    SDL_FRect getViewportRect() const {return m_viewportRect;}

    /**
     * @brief Get the rectangle used by the particles
     * @return SDL_FRect representing the map without the corners
     */
    SDL_FRect getParticlesRect() const {return m_particlesRect;}

    /**
     * @brief Set the texture for the map
     * @details Set the texture for the map AND the viewport/particles rectangles
     * @param renderer SDL_Renderer to render to
     */
    void setTexture(SDL_Renderer* renderer);

    /**
     * @brief Set the number of columns of the map
     * @param renderer SDL_Renderer to render to
     * @param nbColumns Number of columns we want our map to be
     */
    void setNBColumns(SDL_Renderer* renderer, int nbColumns) {
        m_nbColumns = nbColumns;
        setTexture(renderer);
    }

    /**
     * @brief Set the number of rows of the map
     * @param renderer SDL_Renderer to render to
     * @param nbRows Number of rows we want our map to be
     */
    void setNBRows(SDL_Renderer* renderer, int nbRows) {
        m_nbRows = nbRows;
        setTexture(renderer);
    }


    /**
     * @brief Render the map according to the current viewport
     * @param renderer SDL_Renderer to render to
     * @param gameViewport Portion of the map currently visible on screen
     */
    void render(SDL_Renderer* renderer, const SDL_FRect gameViewport);

private:
    /// SDL texture representing the map
    SDL_Texture* m_texture{nullptr};

    /// Map from the point of view of the viewport or particles
    SDL_FRect m_viewportRect;
    SDL_FRect m_particlesRect;

    /// Map dimensions in pixels
    int m_nbColumns;
    int m_nbRows;

    const int m_sizeSquare{50};
    const int m_sizeSeparator{3};

    int m_borderSize{5 * m_sizeSquare};
};