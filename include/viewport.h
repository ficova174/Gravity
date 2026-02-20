#pragma once

#include <SDL3/SDL.h>
#include "map.h"

/**
 * @class Viewport
 * @brief Represents the visible portion of the map
 * @details The Viewport defines which part of the Map is currently visible
 *          on the screen. It supports movement and zooming while ensuring
 *          that it stays within the map boundaries.
 * @author Axel LT
 * @since 2026-02-17
 */
class Viewport {
public:
    /**
     * @brief Get the zoom speed factor
     * @return Zoom speed value
     */
    float getZoomSpeed() const {return m_zoomSpeed;}

    /**
     * @brief Get the current viewport rectangle
     * @return SDL_FRect representing the visible area of the map
     */
    SDL_FRect getViewport() const {return m_viewport;}

    /**
     * @brief Set the viewport size
     * @details Adjusts the width and height of the viewport while
     *          ensuring it remains inside the map boundaries.
     * @note We want our viewport to be as big as possible to get an overall view of the map
     * @param map Reference to the map
     * @param screenWidth Screen width in pixels size
     * @param screenWidth Screen height in pixels size
     */
    void setSize(const Map &map, float screenWidth, float screenHeight);


    /**
     * @brief Zoom the viewport in or out
     * @details Adjusts the viewport size based on zoom input,
     *          it zooms towards the center of the screen.
     * @param map Reference to the map
     * @param changex Horizontal zoom influence (e.g. mouse position)
     * @param changey Vertical zoom influence
     */
    void zoom(const Map &map, const float changex, const float changey);

    /**
     * @brief Move the viewport based on input keys
     * @details Translates the viewport according to pressed keys
     *          (you don't have to spam the key) and elapsed time.
     * @param map Reference to the map
     * @param keys Pointer to SDL keyboard state array
     * @param deltaTime Time elapsed since last frame
     */
    void move(const Map &map, const bool *keys, const float deltaTime);

private:
    /// Rectangle defining the visible region of the map
    SDL_FRect m_viewport{0.0f, 0.0f, 0.0f, 0.0f};

    /// Movement speed of the viewport (units per second)
    static constexpr float m_viewportSpeed{500.0f};

    /// Zoom speed factor
    static constexpr float m_zoomSpeed{30.0f};
};
