#include "Heatmap.h"
#include "Life.h"
#include <algorithm>  // for std::clamp

Heatmap::Heatmap(int radius) : m_radius(radius) {}

Color Heatmap::getColor(const Life& life, int x, int y, int z) const {
    float density = life.computeDensity(x, y, z, m_radius);

    // Thresholds mapped into 0 → 0.15
    const float T1  = 0.05f;  // blue -> cyan
    const float T2  = 0.10f;  // cyan -> green
    const float T3  = 0.125f; // green -> yellow
    const float TOP = 0.15f;  // yellow -> red

    Color color;

    if (density < T1) {
        // blue -> cyan
        float t = (T1 > 0.0f) ? (density / T1) : 0.0f;
        color.r = 0.0f;
        color.g = t;
        color.b = 1.0f;
    }
    else if (density < T2) {
        // cyan -> green
        float t = (density - T1) / (T2 - T1);
        color.r = 0.0f;
        color.g = 1.0f;
        color.b = 1.0f - t;
    }
    else if (density < T3) {
        // green -> yellow
        float t = (density - T2) / (T3 - T2);
        color.r = t;
        color.g = 1.0f;
        color.b = 0.0f;
    }
    else {
        // yellow -> red
        float denom = (TOP - T3);
        float t = denom > 0.0f ? (density - T3) / denom : 1.0f;
        t = glm::clamp(t, 0.0f, 1.0f);
        color.r = 1.0f;
        color.g = 1.0f - t;
        color.b = 0.0f;
    }

    return color;

}
