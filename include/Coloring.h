#pragma once
#include <glm/glm.hpp>
#include "Life.h"

struct Color {
    float r, g, b;
};

enum class ColoringPattern { Heatmap, Grayscale };

class Coloring {
public:
    Coloring(int radius = 1, ColoringPattern pattern = ColoringPattern::Heatmap);
    void setPattern(ColoringPattern pattern) { m_pattern = pattern; }

    Color getColor(const Life&, int, int, int) const;

private:
    int m_radius;
    ColoringPattern m_pattern;

    // Original heatmap interpolation using thresholds
    Color densityToHeatmap(float density) const;

    // Black → White interpolation
    Color densityToGrayscale(float density) const;

    Color lerp(Color a, Color b, float t) const {
        return { a.r + (b.r - a.r)*t,
                 a.g + (b.g - a.g)*t,
                 a.b + (b.b - a.b)*t };
    }
};
