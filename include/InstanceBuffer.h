#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class InstanceBuffer {
public:
    InstanceBuffer(size_t maxInstances);
    ~InstanceBuffer();

    void uploadPositions(const std::vector<glm::vec3>& positions);
    void uploadColors(const std::vector<glm::vec3>& colors);

    GLuint getPositionVBO() const { return m_positionVBO; }
    GLuint getColorVBO() const { return m_colorVBO; }

private:
    GLuint m_positionVBO;
    GLuint m_colorVBO;
    size_t m_maxInstances;
};
