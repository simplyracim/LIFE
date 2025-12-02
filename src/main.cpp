#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Life.h"
#include "Cell.h"
#include "InstanceBuffer.h"
#include "Heatmap.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    std::cout << "Starting 3D Game of Life (modular)..." << std::endl;

    // Create SFML window
    sf::Window window(sf::VideoMode({1200, 800}), "3D Game of Life");
    window.setFramerateLimit(60);

    // Initialize GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 0;
    }
    glEnable(GL_DEPTH_TEST);

    // Load shader
    Shader shader;
    if (!shader.loadFromFile("src/shaders/vertex.glsl", "src/shaders/fragment.glsl")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return 0;
    }

    // Create camera
    Camera camera;

    // Grid size
    const int sizeX = 50, sizeY = 50, sizeZ = 50;

    // Instantiate modular components
    Life life(sizeX, sizeY, sizeZ);
    life.randomize();

    Cell cell;  // Cube mesh
    InstanceBuffer instanceBuffer(sizeX * sizeY * sizeZ);
    Heatmap heatmap(1);  // density radius
    Renderer renderer(cell, instanceBuffer, heatmap);

    // Timing
    sf::Clock clock;
    sf::Clock updateClock;
    const float updateInterval = 0.5f;

    // Main loop
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        bool shouldClose = false;

        while (auto eventOpt = window.pollEvent()) {
            eventOpt->visit([&](const auto& event) {
                using T = std::decay_t<decltype(event)>;
                if constexpr (std::is_same_v<T, sf::Event::Closed>) {
                    shouldClose = true;
                } else if constexpr (std::is_same_v<T, sf::Event::KeyPressed>) {
                    if (event.scancode == sf::Keyboard::Scan::Escape)
                        shouldClose = true;
                    else if (event.scancode == sf::Keyboard::Scan::R)
                        life.randomize();
                    else if (event.scancode == sf::Keyboard::Scan::C)
                        life.clear();
                    else if (event.scancode == sf::Keyboard::Scan::Space)
                        life.update();
                }
            });
            camera.handleEvent(eventOpt.value());
        }

        if (shouldClose) {
            window.close();
            break;
        }

        camera.update(deltaTime);

        // Update life automatically
        if (updateClock.getElapsedTime().asSeconds() >= updateInterval) {
            life.update();
            updateClock.restart();
        }

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = static_cast<float>(window.getSize().x) / window.getSize().y;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspect);

        renderer.render(life, shader, view, projection);

        window.display();
    }

    std::cout << "Exiting application..." << std::endl;
    return 0;
}
