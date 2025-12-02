#include "Cell.h"

Cell::Cell() {
    createGeometry();
}

Cell::~Cell() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void Cell::bind() const {
    glBindVertexArray(m_VAO);
}

void Cell::createGeometry() {
    float vertices[] = {
        -0.5f,-0.5f,-0.5f,  1,0,0,
         0.5f,-0.5f,-0.5f,  0,1,0,
         0.5f, 0.5f,-0.5f,  0,0,1,
        -0.5f, 0.5f,-0.5f,  1,1,0,
        -0.5f,-0.5f, 0.5f,  1,0,1,
         0.5f,-0.5f, 0.5f,  0,1,1,
         0.5f, 0.5f, 0.5f,  0.5,0.5,0.5,
        -0.5f, 0.5f, 0.5f,  1,1,1
    };
    unsigned int indices[] = {
        0,1,2,2,3,0,
        4,5,6,6,7,4,
        0,3,7,7,4,0,
        1,2,6,6,5,1,
        0,1,5,5,4,0,
        3,2,6,6,7,3
    };

    glGenVertexArrays(1,&m_VAO);
    glGenBuffers(1,&m_VBO);
    glGenBuffers(1,&m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER,m_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));

    glBindVertexArray(0);
}
