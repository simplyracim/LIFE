#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Cell.h"
#include "InstanceBuffer.h"
#include "Life.h"
#include "Coloring.h"

class Renderer {
public:
    Renderer(Cell& cell, InstanceBuffer& instanceBuffer, Coloring& heatmap);

    void render(const Life& life, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    Cell& m_cell;
    InstanceBuffer& m_instanceBuffer;
    Coloring& m_heatmap;
};