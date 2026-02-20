#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>

#include "viewport.h"
#include "viewportErrors.h"
#include "map.h"

void Viewport::setSize(const Map &map, float screenWidth, float screenHeight) {
    float aspectRatio{screenWidth / screenHeight};

    // Ideally we want the viewport width equal to map.getWidth()
    float idealHeight{map.getWidth() / aspectRatio};

    // Fortunately the map is high enough to respect our viewport aspect ratio
    if (idealHeight < map.getHeight()) {
        m_viewport.w = map.getWidth();
        m_viewport.h = idealHeight;
    }
    else {
        float widthOfShame{map.getHeight() * aspectRatio};
        m_viewport.w = widthOfShame;
        m_viewport.h = map.getHeight();
    }
}

void Viewport::zoom(const Map &map, const float changex, const float changey) {
    const float mapWidth = map.getWidth();
    const float mapHeight = map.getHeight();

    // 500 is arbitrary and represents min height
    bool lessMinHeight{(m_viewport.h + changey < 500.0f)};
    bool moreMaxHeight{(m_viewport.h + changey > mapHeight)};
    bool moreMaxWidth{(m_viewport.w + changex > mapWidth)};

    if (lessMinHeight || moreMaxHeight || moreMaxWidth) {
        return;
    }

    m_viewport.w += changex;
    m_viewport.h += changey;

    m_viewport.x -= changex / 2.0f;
    m_viewport.y -= changey / 2.0f;

    m_viewport.x = std::clamp(m_viewport.x, 0.0f, mapWidth - m_viewport.w);
    m_viewport.y = std::clamp(m_viewport.y, 0.0f, mapHeight - m_viewport.h);
}

void Viewport::move(const Map &map, const bool *keys, const float deltaTime) {
    float dx{0.0f};
    float dy{0.0f};

    if (keys[SDL_SCANCODE_W]) dy -= 1.0f;
    if (keys[SDL_SCANCODE_S]) dy += 1.0f;
    if (keys[SDL_SCANCODE_A]) dx -= 1.0f;
    if (keys[SDL_SCANCODE_D]) dx += 1.0f;

    float length{0.0f};
    length = std::sqrt(dx * dx + dy * dy);

    if (length > 0.0f) {
        if (dx != 0.0f) {
            m_viewport.x += (m_viewportSpeed * deltaTime * dx) / length;
        }
        if (dy != 0.0f) {
            m_viewport.y += (m_viewportSpeed * deltaTime * dy) / length;
        }
    }

    m_viewport.x = std::clamp(m_viewport.x, 0.0f, map.getWidth() - m_viewport.w);
    m_viewport.y = std::clamp(m_viewport.y, 0.0f, map.getHeight() - m_viewport.h);
}
