#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader() : m_program(0) {}

Shader::~Shader() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

std::string Shader::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);
    
    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);
    
    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cerr << "Failed to read shader files: " << vertexPath << " or " << fragmentPath << std::endl;
        return false;
    }
    
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    if (!vertexShader || !fragmentShader) {
        std::cerr << "Failed to compile shaders" << std::endl;
        return false;
    }
    
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);
    
    // Check linking status
    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(m_program);
        m_program = 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return m_program != 0;
}

void Shader::use() {
    glUseProgram(m_program);
}

void Shader::setUniform(const std::string& name, float value) {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::setUniform(const std::string& name, int value) {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::setUniform(const std::string& name, const sf::Vector3f& value) {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniform3f(location, value.x, value.y, value.z);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniform3f(location, value.x, value.y, value.z);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}