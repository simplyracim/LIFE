#pragma once
#include <string>
#include <glad/glad.h>  // Include GLAD first
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();
    
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, const sf::Vector3f& value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::mat4& value);

private:
    GLuint m_program;
    
    GLuint compileShader(const std::string& source, GLenum type);
    std::string readFile(const std::string& filePath);
};