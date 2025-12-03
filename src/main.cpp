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
#include "gui/Button.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    std::cout << "Starting 3D Game of Life..." << std::endl;

    // Create SFML window (render) for OpenGL + 2D overlay
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "3D Game of Life");

    window.setFramerateLimit(60);

    // Initialize GLAD / OpenGL
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        return 1;
    }
    glEnable(GL_DEPTH_TEST);

    // Load shader
    Shader shader;
    if (!shader.loadFromFile("src/shaders/vertex.glsl", "src/shaders/fragment.glsl")) {
        std::cerr << "Failed to load shaders\n";
        return 1;
    }

    // Load font (SFML 3 uses openFromFile or ctor-from-path)
    sf::Font font;
    if (!font.openFromFile("assets/consolas-regular.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }
    
    // Grid size
    const int sizeX = 30, sizeY = 30, sizeZ = 30;

    // Instantiate core modules
    Life life(sizeX, sizeY, sizeZ);
    life.randomize();

    Cell cell;
    InstanceBuffer instanceBuffer(sizeX * sizeY * sizeZ);
    Heatmap heatmap(5);
    Renderer renderer(cell, instanceBuffer, heatmap);


    // Create buttons
    std::vector<Button> buttons;
    buttons.emplace_back(sf::Vector2f(20.f, 20.f), sf::Vector2f(120.f, 40.f), font,
                        "Randomize", 18, [&]() { life.randomize(); });
    buttons.emplace_back(sf::Vector2f(20.f, 70.f), sf::Vector2f(120.f, 40.f), font,
                        "Clear", 18, [&]() { life.clear(); });
    buttons.emplace_back(sf::Vector2f(20.f, 120.f), sf::Vector2f(120.f, 40.f), font,
                        "Update", 18, [&]() { life.update(); });

    // Create camera
    Camera camera;

    // Timing
    sf::Clock clock;
    sf::Clock updateClock;
    const float updateInterval = 0.5f;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Process events (SFML 3 uses std::optional)
        while (auto evOpt = window.pollEvent()) {
            const sf::Event &event = *evOpt;

            // Window closed
            if (event.is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            // Keyboard
            if (auto kp = event.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    window.close();
                    break;
                }
                else if (kp->code == sf::Keyboard::Key::C) {
                    life.clear();
                }
                else if (kp->code == sf::Keyboard::Key::Space) {
                    life.update();
                }
            }

            if (event.is<sf::Event::MouseButtonPressed>()) {
                auto btn = event.getIf<sf::Event::MouseButtonPressed>();
                if (btn->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(btn->position.x),
                      static_cast<float>(btn->position.y));
                    for (auto& button : buttons)
                        button.handleClick(mousePos);
                }
            }

            // Pass event to camera
            camera.handleEvent(event);
        }

        // Update camera
        camera.update(deltaTime);

        // Automatic Life update
        if (updateClock.getElapsedTime().asSeconds() >= updateInterval) {
            life.update();
            updateClock.restart();
        }

        // 3D render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = static_cast<float>(window.getSize().x) / window.getSize().y;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspect);

        renderer.render(life, shader, view, projection);

        window.pushGLStates();
        for (auto& button : buttons)
            button.draw(window);
        window.popGLStates();

        window.display();
    }

    std::cout << "Exiting application..." << std::endl;
    return 0;
}
