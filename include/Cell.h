#pragma once
#include <glad/glad.h>

class Cell {
public:
    Cell();
    ~Cell();

    void bind() const;

private:
    GLuint m_VAO, m_VBO, m_EBO;
    void createGeometry();
};
