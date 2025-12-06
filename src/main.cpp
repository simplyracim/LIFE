#include "Shader.h"
#include "Camera.h"
#include "Life.h"
#include "Cell.h"
#include "InstanceBuffer.h"
#include "Coloring.h"
#include "Renderer.h"

// GUI
#include "gui/Panel.h"
#include "gui/Button.h"

#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    std::cout << "Starting 3D Game of Life..." << std::endl;

    // Window
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "3D Game of Life");
    window.setFramerateLimit(60);

    auto W = window.getSize().x;
    auto H = window.getSize().y;

    // GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        return 1;
    }
    glEnable(GL_DEPTH_TEST);

    // Shaders
    Shader shader;
    if (!shader.loadFromFile("src/shaders/vertex.glsl", "src/shaders/fragment.glsl")) {
        std::cerr << "Failed to load shaders\n";
        return 1;
    }

    // Font
    sf::Font font;
    if (!font.openFromFile("assets/consolas-regular.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    // Grid size
    const int sizeX = 30, sizeY = 30, sizeZ = 30;

    // Game of Life core
    Life life(sizeX, sizeY, sizeZ);
    life.randomize();

    // Rendering modules
    Cell cell;
    InstanceBuffer instanceBuffer(sizeX * sizeY * sizeZ);
    Coloring heatmap(5);
    Renderer renderer(cell, instanceBuffer, heatmap);

    // GUI Panel
    float panelY = H * 3.f / 4.f;
    float panelHeight = H / 4.f;

    float oneThird = W / 3.f;
    float oneSixth = W / 6.f;

    // --- Middle Panel (same as before) ---
    Panel middlePanel(
        sf::Vector2f(oneThird, panelY),
        sf::Vector2f(oneThird, panelHeight),
        font,
        "Controls"
    );

    // --- Right: 2 panels side by side ---
    Panel rightPanel1(
        sf::Vector2f(2.f * oneThird, panelY),
        sf::Vector2f(oneSixth, panelHeight),
        font,
        ""
    );

    Panel rightPanel2(
        sf::Vector2f(2.f * oneThird + oneSixth, panelY),
        sf::Vector2f(oneSixth, panelHeight),
        font,
        ""
    );


/*
Button::Button(const sf::Vector2f& pos, const sf::Vector2f& size,
               const sf::Font& font, const std::string& textStr,
               unsigned int charSize, std::function<void()> callback)
    : m_text(font), m_onClick(callback)
*/

// Media control buttons at the top of the panel
float mediaButtonSize = 40.f;   // square buttons
float padding = 0.f;            // spacing between buttons
float startX = padding;
float startY = padding;         // relative to panel top

std::vector<std::string> symbols = {"<<", "<", "||", ">", ">>"};

for (size_t i = 0; i < symbols.size(); ++i) {
    middlePanel.addButton(Button(
        sf::Vector2f(startX + i * (mediaButtonSize + padding), startY),
        sf::Vector2f(mediaButtonSize, mediaButtonSize),
        font,
        symbols[i],
        16,
        nullptr // no callback yet
    ));
}

// Add all buttons
middlePanel.addButton(Button(
    sf::Vector2f(20.f, 20.f), sf::Vector2f(120.f, 40.f),
    font, "Randomize", 18,
    [&]() { life.randomize(); }
));

middlePanel.addButton(Button(
    sf::Vector2f(20.f, 70.f), sf::Vector2f(120.f, 40.f),
    font, "Clear", 18,
    [&]() { life.clear(); }
));

middlePanel.addButton(Button(
    sf::Vector2f(20.f, 120.f), sf::Vector2f(120.f, 40.f),
    font, "Update", 18,
    [&]() { life.update(); }
));

// Additional test buttons
middlePanel.addButton(Button(sf::Vector2f(20.f, 170.f), sf::Vector2f(120.f, 40.f), font, "Test 1", 18, nullptr));
middlePanel.addButton(Button(sf::Vector2f(20.f, 220.f), sf::Vector2f(120.f, 40.f), font, "Test 2", 18, nullptr));
middlePanel.addButton(Button(sf::Vector2f(20.f, 270.f), sf::Vector2f(120.f, 40.f), font, "Test 3", 18, nullptr));
middlePanel.addButton(Button(sf::Vector2f(20.f, 320.f), sf::Vector2f(120.f, 40.f), font, "Test 4", 18, nullptr));

middlePanel.layoutButtons();

// --- Right panel 1 buttons (4 buttons like middlePanel) ---
rightPanel1.addButton(Button(sf::Vector2f(20.f, 20.f), sf::Vector2f(83.f, 83.f), font, "RA1", 16,
    [&]() { life.setMode(LifeMode::Current3D); }));

rightPanel1.addButton(Button(sf::Vector2f(20.f, 70.f), sf::Vector2f(83.f, 83.f), font, "RA2", 16,
    [&]() { life.setMode(LifeMode::Conway2D); }));

rightPanel1.addButton(Button(sf::Vector2f(20.f, 120.f), sf::Vector2f(83.f, 83.f), font, "RA3", 16,
    [&]() { life.setMode(LifeMode::Custom3D); }));

rightPanel1.addButton(Button(sf::Vector2f(20.f, 170.f), sf::Vector2f(83.f, 83.f), font, "RA4", 16,
    [&]() { life.setMode(LifeMode::Custom2D); }));


rightPanel1.layoutButtons();


// --- Right panel 2 buttons (4 buttons like middlePanel) ---
rightPanel2.addButton(Button(sf::Vector2f(20.f, 20.f), sf::Vector2f(83.f, 83.f), font, "RB1", 16,
    [&]() { heatmap.setPattern(ColoringPattern::Heatmap); })); // RB1 → Heatmap

rightPanel2.addButton(Button(sf::Vector2f(20.f, 70.f), sf::Vector2f(83.f, 83.f), font, "RB2", 16,
    [&]() { heatmap.setPattern(ColoringPattern::Grayscale); })); // RB2 → Grayscale

rightPanel2.addButton(Button(sf::Vector2f(20.f, 120.f), sf::Vector2f(83.f, 83.f), font, "RB3", 16, nullptr));
rightPanel2.addButton(Button(sf::Vector2f(20.f, 170.f), sf::Vector2f(83.f, 83.f), font, "RB4", 16, nullptr));

rightPanel2.layoutButtons();



    // Camera
    Camera camera;

    // Timing
    sf::Clock clock;
    sf::Clock updateClock;
    const float updateInterval = 0.5f;

    // Event loop
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (auto evOpt = window.pollEvent()) {
            const sf::Event& event = *evOpt;

            if (event.is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            // Keyboard events
            if (auto kp = event.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape)
                    window.close();
                else if (kp->code == sf::Keyboard::Key::C)
                    life.clear();
                else if (kp->code == sf::Keyboard::Key::Space)
                    life.update();
            }

            // Mouse click → delegate to panel
            if (auto mp = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mp->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mp->position.x),
                                         static_cast<float>(mp->position.y));
                    middlePanel.handleClick(mousePos);
                    rightPanel1.handleClick(mousePos);
                    rightPanel2.handleClick(mousePos);
                }
            }

            // Camera events
            camera.handleEvent(event);
        }

        // Update camera
        camera.update(deltaTime);

        // Automatic Life update
        if (updateClock.getElapsedTime().asSeconds() >= updateInterval) {
            life.update();
            updateClock.restart();
        }

        // OpenGL Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = static_cast<float>(window.getSize().x) / window.getSize().y;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspect);

        renderer.render(life, shader, view, projection);

        // Draw GUI
        window.pushGLStates();
        middlePanel.render(window);
        rightPanel1.render(window);
        rightPanel2.render(window);
        window.popGLStates();

        window.display();
    }

    std::cout << "Exiting application..." << std::endl;
    return 0;
}
