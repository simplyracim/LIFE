#include "Grid3D.h"
#include "Shader.h"
#include <random>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>  // Include GLAD

Grid3D::Grid3D(int sizeX, int sizeY, int sizeZ)
    : m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ),
      m_VAO(0), m_VBO(0), m_EBO(0),
      m_instanceVBO(0), m_colorVBO(0)
{
    m_grid.resize(m_sizeX * m_sizeY * m_sizeZ, false);
    m_nextGrid.resize(m_sizeX * m_sizeY * m_sizeZ, false);

    // Reserve instance vectors for performance
    m_instancePositions.reserve(m_sizeX * m_sizeY * m_sizeZ);
    m_instanceColors.reserve(m_sizeX * m_sizeY * m_sizeZ);

    createCubeGeometry();
}

Grid3D::~Grid3D() {
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
    }
    if (m_EBO) {
        glDeleteBuffers(1, &m_EBO);
    }
    if (m_instanceVBO) {
        glDeleteBuffers(1, &m_instanceVBO);
    }
    if (m_colorVBO) {
        glDeleteBuffers(1, &m_colorVBO);
    }
}

void Grid3D::createCubeGeometry() {
    // Cube vertices (positions and colors) - 8 vertices, 6 floats each (pos + color)
    float vertices[] = {
        // Positions          // Colors (unused for instanced color but kept)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // 0
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // 1
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // 2
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  // 3
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  // 4
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  // 5
         0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  // 6
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f   // 7
    };

    unsigned int indices[] = {
        // Back face
        0, 1, 2, 2, 3, 0,
        // Front face
        4, 5, 6, 6, 7, 4,
        // Left face
        0, 3, 7, 7, 4, 0,
        // Right face
        1, 2, 6, 6, 5, 1,
        // Bottom face
        0, 1, 5, 5, 4, 0,
        // Top face
        3, 2, 6, 6, 7, 3
    };

    // Create VAO, VBO, EBO and instance buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenBuffers(1, &m_instanceVBO);
    glGenBuffers(1, &m_colorVBO);

    glBindVertexArray(m_VAO);

    // Upload cube vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex positions (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // Vertex colors (location = 1) -- we keep this but we'll use per-instance color instead
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // ----- Instance attributes -----
    // Allocate max instance buffer size once (max instances = grid size)
    size_t maxInstances = static_cast<size_t>(m_sizeX) * m_sizeY * m_sizeZ;
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    // instance position (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(2, 1);

    // instance color (location = 3)
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);

    // Unbind VAO
    glBindVertexArray(0);
}

void Grid3D::initialize() {
    randomize();
}

void Grid3D::update() {
    // Apply Conway's Game of Life rules in 3D
    for (int z = 0; z < m_sizeZ; ++z) {
        for (int y = 0; y < m_sizeY; ++y) {
            for (int x = 0; x < m_sizeX; ++x) {
                int neighbors = countNeighbors(x, y, z);
                bool currentState = getCell(x, y, z);

                // 3D Conway rules (modified from 2D)
                if (currentState) {
                    // Cell survives with 5-6 neighbors (3D adaptation)
                    m_nextGrid[getIndex(x, y, z)] = (neighbors == 5 || neighbors == 6);
                } else {
                    // Cell is born with exactly 5 neighbors
                    m_nextGrid[getIndex(x, y, z)] = (neighbors == 5);
                }
            }
        }
    }

    std::swap(m_grid, m_nextGrid);
}

void Grid3D::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);

    // Rebuild instance arrays
    m_instancePositions.clear();
    m_instanceColors.clear();

    // Build instances from alive cells and compute density -> grayscale color
    const int radius = m_heatmapRadius; // integer radius

    for (int z = 0; z < m_sizeZ; ++z) {
        for (int y = 0; y < m_sizeY; ++y) {
            for (int x = 0; x < m_sizeX; ++x) {
                if (!getCell(x, y, z)) continue;

                // Center position so grid is centered at origin
                float offsetX = x - m_sizeX / 2.0f;
                float offsetY = y - m_sizeY / 2.0f;
                float offsetZ = z - m_sizeZ / 2.0f;
                m_instancePositions.emplace_back(offsetX, offsetY, offsetZ);

                float density = computeDensity(x, y, z, radius); // 0..1

                // Map density to grayscale white: higher density -> closer to white (1.0)
                glm::vec3 col(density); // same value in r,g,b
                m_instanceColors.push_back(col);
            }
        }
    }

    // If there are no instances, nothing to draw
    GLsizei instanceCount = static_cast<GLsizei>(m_instancePositions.size());
    if (instanceCount == 0) return;

    // Upload instance positions
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::vec3), m_instancePositions.data());

    // Upload instance colors
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::vec3), m_instanceColors.data());

    // Draw
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instanceCount);
    glBindVertexArray(0);
}

void Grid3D::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int z = 0; z < m_sizeZ; ++z) {
        for (int y = 0; y < m_sizeY; ++y) {
            for (int x = 0; x < m_sizeX; ++x) {
                m_grid[getIndex(x, y, z)] = (dis(gen) > 0.7f);
            }
        }
    }
}

void Grid3D::clear() {
    std::fill(m_grid.begin(), m_grid.end(), false);
}

void Grid3D::setCell(int x, int y, int z, bool state) {
    if (isValidPosition(x, y, z)) {
        m_grid[getIndex(x, y, z)] = state;
    }
}

bool Grid3D::getCell(int x, int y, int z) const {
    if (isValidPosition(x, y, z)) {
        return m_grid[getIndex(x, y, z)];
    }
    return false;
}

int Grid3D::countNeighbors(int x, int y, int z) const {
    int count = 0;

    // Check all 26 neighbors in 3D
    for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0 && dz == 0) continue; // Skip the cell itself

                int nx = x + dx;
                int ny = y + dy;
                int nz = z + dz;

                if (isValidPosition(nx, ny, nz) && getCell(nx, ny, nz)) {
                    count++;
                }
            }
        }
    }

    return count;
}

float Grid3D::computeDensity(int x, int y, int z, int radius) const {
    int count = 0;
    int total = 0;

    for (int dz = -radius; dz <= radius; ++dz) {
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                int nz = z + dz;

                if (!isValidPosition(nx, ny, nz)) continue;
                if (dx == 0 && dy == 0 && dz == 0) continue; // skip self

                total++;
                if (getCell(nx, ny, nz)) count++;
            }
        }
    }

    if (total == 0) return 0.0f;
    return static_cast<float>(count) / static_cast<float>(total); // normalized 0..1
}

int Grid3D::getIndex(int x, int y, int z) const {
    return z * m_sizeX * m_sizeY + y * m_sizeX + x;
}

bool Grid3D::isValidPosition(int x, int y, int z) const {
    return x >= 0 && x < m_sizeX && y >= 0 && y < m_sizeY && z >= 0 && z < m_sizeZ;
}
