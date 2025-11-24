#include "Grid3D.h"
#include "Shader.h"
#include <random>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>  // Include GLAD

Grid3D::Grid3D(int sizeX, int sizeY, int sizeZ) 
    : m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ),
      m_VAO(0), m_VBO(0), m_EBO(0)  // Initialize OpenGL objects to 0
{
    m_grid.resize(m_sizeX * m_sizeY * m_sizeZ, false);
    m_nextGrid.resize(m_sizeX * m_sizeY * m_sizeZ, false);
    
    createCubeGeometry();
}

Grid3D::~Grid3D() {
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
}

void Grid3D::createCubeGeometry() {
    // Cube vertices (positions and colors)
    float vertices[] = {
        // Positions          // Colors
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // red
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // green
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // blue
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  // yellow
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  // magenta
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  // cyan
         0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  // gray
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f   // white
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
    
    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Set vertex attributes
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
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
    
    glBindVertexArray(m_VAO);
    
    // Render each alive cell as a cube
    for (int z = 0; z < m_sizeZ; ++z) {
        for (int y = 0; y < m_sizeY; ++y) {
            for (int x = 0; x < m_sizeX; ++x) {
                if (getCell(x, y, z)) {
                    // Calculate world position
                    float offsetX = x - m_sizeX / 2.0f;
                    float offsetY = y - m_sizeY / 2.0f;
                    float offsetZ = z - m_sizeZ / 2.0f;
                    
                    // Create model matrix using GLM
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, offsetZ));
                    shader.setUniform("model", model);
                    
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                }
            }
        }
    }
    
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

int Grid3D::countNeighbors(int x, int y, int z) {
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

int Grid3D::getIndex(int x, int y, int z) const {
    return z * m_sizeX * m_sizeY + y * m_sizeX + x;
}

bool Grid3D::isValidPosition(int x, int y, int z) const {
    return x >= 0 && x < m_sizeX && y >= 0 && y < m_sizeY && z >= 0 && z < m_sizeZ;
}