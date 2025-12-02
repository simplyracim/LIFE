#pragma once
#include <glm/glm.hpp>
#include "Life.h"

class Heatmap {
public:
    Heatmap(int radius = 1);
    glm::vec3 getColor(const Life& life, int x, int y, int z) const;

private:
    int m_radius;
};
