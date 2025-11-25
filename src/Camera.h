#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    
    void update(float deltaTime);
    void handleEvent(const sf::Event& event);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    
    float m_yaw;
    float m_pitch;
    float m_distance;
    
    bool m_isRotating;
    sf::Vector2i m_lastMousePos;
};

#endif // CAMERA_H