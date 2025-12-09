#include "Coloring.h"
#include "Life.h"
#include <algorithm> // for std::clamp
#include <cmath>     // for sin()

Coloring::Coloring(int radius, ColoringPattern pattern) 
    : m_radius(radius), m_pattern(pattern) {}

// -------------------------------------------------------------
// Heatmap with thresholds T1 → T2 → T3 → TOP
// -------------------------------------------------------------
Color Coloring::densityToHeatmap(float density) const {
    const float T1  = 0.05f;  // blue -> cyan
    const float T2  = 0.10f;  // cyan -> green
    const float T3  = 0.125f; // green -> yellow
    const float TOP = 0.15f;  // yellow -> red

    Color color;

    if (density < T1) {
        float t = (T1 > 0.0f) ? (density / T1) : 0.0f;
        color.r = 0.0f;
        color.g = t;
        color.b = 1.0f;
    }
    else if (density < T2) {
        float t = (density - T1) / (T2 - T1);
        color.r = 0.0f;
        color.g = 1.0f;
        color.b = 1.0f - t;
    }
    else if (density < T3) {
        float t = (density - T2) / (T3 - T2);
        color.r = t;
        color.g = 1.0f;
        color.b = 0.0f;
    }
    else {
        float denom = (TOP - T3);
        float t = denom > 0.0f ? (density - T3) / denom : 1.0f;
        t = glm::clamp(t, 0.0f, 1.0f);
        color.r = 1.0f;
        color.g = 1.0f - t;
        color.b = 0.0f;
    }

    return color;
}

// -------------------------------------------------------------
// Grayscale: black → white
// -------------------------------------------------------------
Color Coloring::densityToGrayscale(float density) const {
    float v = glm::clamp(density / 0.15f, 0.0f, 1.0f); // normalize to 0→1
    return { v, v, v };
}

// -------------------------------------------------------------
// NEW: Fade from back (z=0) to front (z=max) : black → white
// -------------------------------------------------------------
Color Coloring::zFadeColor(const Life& life, int z) const {
    int maxZ = life.getSizeZ() - 1;
    if (maxZ <= 0)
        return {0.0f, 0.0f, 0.0f};

    float t = float(z) / float(maxZ);
    return { t, t, t };
}

// -------------------------------------------------------------
// NEW: Blue pulse based on density (shimmering effect)
// -------------------------------------------------------------
Color Coloring::bluePulseColor(float density) const {
    float r = 0.2f + density * 2.0f;
    float g = 0.2f;
    float b = 0.5f + 0.5f * std::sin(density * 40.0f);

    r = glm::clamp(r, 0.0f, 1.0f);
    b = glm::clamp(b, 0.0f, 1.0f);

    return { r, g, b };
}

// -------------------------------------------------------------
// Main selector
// -------------------------------------------------------------
Color Coloring::getColor(const Life& life, int x, int y, int z) const {
    float density = life.computeDensity(x, y, z, m_radius);

    switch (m_pattern) {
        case ColoringPattern::Heatmap:
            return densityToHeatmap(density);

        case ColoringPattern::Grayscale:
            return densityToGrayscale(density);

        case ColoringPattern::ZFade:
            return zFadeColor(life, z);

        case ColoringPattern::BluePulse:
            return bluePulseColor(density);
    }

    return { 1.0f, 0.0f, 1.0f }; // fallback magenta
}
