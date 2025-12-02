#pragma once
#include <glm/glm.hpp>
#include "Life.h"

struct Color {
    float r, g, b;
};

class Heatmap {
public:
    Heatmap(int radius = 1);
    Color getColor(const Life&, int, int, int) const;

private:
    int m_radius;

    Color densityToColor(float density) const {
        if (density < 0.25f)
            return lerp({0.0f,0.0f,1.0f}, {0.0f,1.0f,0.0f}, density/0.25f); // Blue → Green
        else if (density < 0.5f)
            return lerp({0.0f,1.0f,0.0f}, {1.0f,1.0f,0.0f}, (density-0.25f)/0.25f); // Green → Yellow
        else if (density < 0.75f)
            return lerp({1.0f,1.0f,0.0f}, {1.0f,0.5f,0.0f}, (density-0.5f)/0.25f); // Yellow → Orange
        else
            return lerp({1.0f,0.5f,0.0f}, {1.0f,0.0f,0.0f}, (density-0.75f)/0.25f); // Orange → Red
    }

    Color lerp(Color a, Color b, float t) const {
        return { a.r + (b.r - a.r)*t,
                 a.g + (b.g - a.g)*t,
                 a.b + (b.b - a.b)*t };
    }
};
