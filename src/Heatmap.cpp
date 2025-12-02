#include "Heatmap.h"
#include "Life.h"
#include <algorithm>  // for std::clamp

Heatmap::Heatmap(int radius) : m_radius(radius) {}

Color Heatmap::getColor(const Life& life, int x, int y, int z) const {
    float density = life.computeDensity(x, y, z, m_radius);

    // Map density [0,1] to a color gradient
    Color color;
    if (density < 0.25f) {
        // blue to cyan
        color.r = 0.0f;
        color.g = density * 4.0f;
        color.b = 1.0f;
    } else if (density < 0.5f) {
        // cyan to green
        float t = (density - 0.25f) * 4.0f;
        color.r = 0.0f;
        color.g = 1.0f;
        color.b = 1.0f - t;
    } else if (density < 0.75f) {
        // green to yellow
        float t = (density - 0.5f) * 4.0f;
        color.r = t;
        color.g = 1.0f;
        color.b = 0.0f;
    } else {
        // yellow to red
        float t = (density - 0.75f) * 4.0f;
        color.r = 1.0f;
        color.g = 1.0f - t;
        color.b = 0.0f;
    }

    return color;
}
