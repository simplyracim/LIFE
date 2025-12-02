#include "Heatmap.h"

Heatmap::Heatmap(int radius) : m_radius(radius) {}

glm::vec3 Heatmap::getColor(const Life& life, int x, int y, int z) const {
    float density = life.computeDensity(x, y, z, m_radius);
    return glm::vec3(density); // grayscale
}
