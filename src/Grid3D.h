#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>  // Include GLAD for OpenGL types

class Shader;

class Grid3D {
public:
    Grid3D(int sizeX, int sizeY, int sizeZ);
    ~Grid3D();
    
    void initialize();
    void update();
    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    
    void randomize();
    void clear();
    void setCell(int x, int y, int z, bool state);
    bool getCell(int x, int y, int z) const;
    
    int getSizeX() const { return m_sizeX; }
    int getSizeY() const { return m_sizeY; }
    int getSizeZ() const { return m_sizeZ; }

private:
    int m_sizeX, m_sizeY, m_sizeZ;
    std::vector<bool> m_grid;
    std::vector<bool> m_nextGrid;
    
    // OpenGL resources
    GLuint m_VAO, m_VBO, m_EBO;
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    
    void createCubeGeometry();
    int countNeighbors(int x, int y, int z);
    int getIndex(int x, int y, int z) const;
    bool isValidPosition(int x, int y, int z) const;
};