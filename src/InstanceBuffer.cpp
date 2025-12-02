#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer(size_t maxInstances)
    : m_maxInstances(maxInstances)
{
    glGenBuffers(1, &m_positionVBO);
    glGenBuffers(1, &m_colorVBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
}

InstanceBuffer::~InstanceBuffer() {
    if (m_positionVBO) glDeleteBuffers(1, &m_positionVBO);
    if (m_colorVBO) glDeleteBuffers(1, &m_colorVBO);
}

void InstanceBuffer::uploadPositions(const std::vector<glm::vec3>& positions) {
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());
}

void InstanceBuffer::uploadColors(const std::vector<glm::vec3>& colors) {
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(glm::vec3), colors.data());
}
