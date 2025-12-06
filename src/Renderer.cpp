#include "Renderer.h"

Renderer::Renderer(Cell& cell, InstanceBuffer& instanceBuffer, Coloring& heatmap)
    : m_cell(cell), m_instanceBuffer(instanceBuffer), m_heatmap(heatmap)
{}

void Renderer::render(const Life& life, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;

    int sizeX = life.getSizeX();
    int sizeY = life.getSizeY();
    int sizeZ = life.getSizeZ();

    for (int z = 0; z < sizeZ; ++z)
        for (int y = 0; y < sizeY; ++y)
            for (int x = 0; x < sizeX; ++x) {
                if (!life.getCell(x, y, z)) continue;

                float offsetX = x - sizeX / 2.0f;
                float offsetY = y - sizeY / 2.0f;
                float offsetZ = z - sizeZ / 2.0f;

                positions.emplace_back(offsetX, offsetY, offsetZ);

                Color c = m_heatmap.getColor(life, x, y, z);
                colors.emplace_back(c.r, c.g, c.b);
            }

    if (positions.empty()) return;

    // Upload positions and colors to GPU
    m_instanceBuffer.uploadPositions(positions);
    m_instanceBuffer.uploadColors(colors);

    // Bind cell VAO and setup instanced attributes
    m_cell.bind();

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer.getPositionVBO());
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer.getColorVBO());
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);

    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, positions.size());

    glBindVertexArray(0);
}