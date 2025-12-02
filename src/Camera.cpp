#include "Camera.h"
#include <cmath>
#include <SFML/Window/Mouse.hpp>

Camera::Camera() 
    : m_position(0.0f, 0.0f, 5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
    , m_distance(100.0f)
    , m_isRotating(false) {
}

void Camera::update(float deltaTime) {
    // Update camera position based on spherical coordinates
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    
    m_position.x = m_distance * std::cos(yawRad) * std::cos(pitchRad);
    m_position.y = m_distance * std::sin(pitchRad);
    m_position.z = m_distance * std::sin(yawRad) * std::cos(pitchRad);
};

void Camera::handleEvent(const sf::Event& event) {
    // SFML 3.0 uses visitor pattern
    event.visit([this](const auto& event) {
        using T = std::decay_t<decltype(event)>;
        
        if constexpr (std::is_same_v<T, sf::Event::MouseButtonPressed>) {
            if (event.button == sf::Mouse::Button::Left) {
                m_isRotating = true;
                m_lastMousePos = sf::Vector2i(event.position.x, event.position.y);
            }
        }
        else if constexpr (std::is_same_v<T, sf::Event::MouseButtonReleased>) {
            if (event.button == sf::Mouse::Button::Left) {
                m_isRotating = false;
            }
        }
        else if constexpr (std::is_same_v<T, sf::Event::MouseMoved>) {
            if (m_isRotating) {
                sf::Vector2i currentPos(event.position.x, event.position.y);
                sf::Vector2i delta = currentPos - m_lastMousePos;
                
                m_yaw += delta.x * 0.3f;
                m_pitch += delta.y * 0.3f;
                
                // Clamp pitch to avoid flipping
                if (m_pitch > 89.0f) m_pitch = 89.0f;
                if (m_pitch < -89.0f) m_pitch = -89.0f;
                
                m_lastMousePos = currentPos;
            }
        }
        else if constexpr (std::is_same_v<T, sf::Event::MouseWheelScrolled>) {
            m_distance -= event.delta * m_distance / 50;
            if (m_distance < m_distance / 100) m_distance = m_distance / 100;
            if (m_distance > m_distance * 100) m_distance = m_distance * 100;
        }
    });
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
}

void Camera::setTarget(const glm::vec3& target) {
    m_target = target;
}