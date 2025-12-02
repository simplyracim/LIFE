#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Grid3D.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    std::cout << "Starting 3D Game of Life..." << std::endl;

    // Create window with SFML 3.0
    sf::Window window(sf::VideoMode({1200, 800}), "3D Game of Life");
    window.setFramerateLimit(60);

    std::cout << "Window created successfully" << std::endl;

    // Initialize GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 0;
    }

    std::cout << "GLAD initialized successfully" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    std::cout << "OpenGL state configured" << std::endl;

    // Create shader
    Shader shader;
    if (!shader.loadFromFile("src/shaders/vertex.glsl", "src/shaders/fragment.glsl")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return 0;
    }
    std::cout << "Shaders loaded successfully" << std::endl;

    // Create camera
    Camera camera;
    std::cout << "Camera created" << std::endl;

    // Create 3D grid
    Grid3D grid(50, 50, 50);  // Smaller grid for better performance
    grid.initialize();
    std::cout << "Grid initialized" << std::endl;

    // Game loop
    sf::Clock clock;
    sf::Clock updateClock;
    const float updateInterval = .5f;

    std::cout << "Entering main loop..." << std::endl;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Handle events - SFML 3.0 style
        bool shouldClose = false;
        
        while (auto eventOpt = window.pollEvent()) {
            // Use the visitor pattern to handle events
            eventOpt->visit([&](const auto& event) {
                using T = std::decay_t<decltype(event)>;
                
                // Handle closed event
                if constexpr (std::is_same_v<T, sf::Event::Closed>) {
                    shouldClose = true;
                }
                // Handle key pressed events
                else if constexpr (std::is_same_v<T, sf::Event::KeyPressed>) {
                    if (event.scancode == sf::Keyboard::Scan::Escape) {
                        shouldClose = true;
                    }
                    else if (event.scancode == sf::Keyboard::Scan::R) {
                        std::cout << "Randomizing grid..." << std::endl;
                        grid.randomize();
                    }
                    else if (event.scancode == sf::Keyboard::Scan::C) {
                        std::cout << "Clearing grid..." << std::endl;
                        grid.clear();
                    }
                    else if (event.scancode == sf::Keyboard::Scan::Space) {
                        std::cout << "Manual update..." << std::endl;
                        grid.update();
                    }
                }
            });
            
            // Pass event to camera
            camera.handleEvent(eventOpt.value());
        }

        if (shouldClose) {
            window.close();
            break;
        }

        // Update camera
        camera.update(deltaTime);

        // Update grid at intervals
        if (updateClock.getElapsedTime().asSeconds() >= updateInterval) {
            grid.update();
            updateClock.restart();
        }

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get matrices
        float aspectRatio = static_cast<float>(window.getSize().x) / window.getSize().y;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);

        // Render grid
        grid.render(shader, view, projection);

        // Display
        window.display();
    }

    std::cout << "Exiting application..." << std::endl;
    return 0;
}