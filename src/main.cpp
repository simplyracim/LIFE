
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
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <atomic>
#include <string>

std::vector<std::string> listStartingConfigs(const std::string& folder = "IO") {
    std::vector<std::string> configs;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(folder)) {
            if (entry.is_directory()) {
                configs.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error reading folder " << folder << ": " << e.what() << "\n";
    }

    return configs;
}

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
    if (!shader.loadFromFile("shaders/vertex.glsl", "shaders/fragment.glsl")) {
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

    Panel middlePanel(
        sf::Vector2f(oneThird, panelY),
        sf::Vector2f(oneThird, panelHeight),
        font,
        ""
    );

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

// Timing
sf::Clock clock;
sf::Clock updateClock;
float updateInterval = 0.5f; // base interval
float simSpeed = 1.f;        // current simulation speed

// Media control buttons
float mediaButtonSize = 40.f;
float padding = 5.f;
float startX = 5.f;
float startY = 5.f;

std::vector<std::string> symbols = {"Start", "Stop", ">", ">>"};

for (size_t i = 0; i < symbols.size(); ++i) {
    middlePanel.addButton(Button(
        sf::Vector2f(startX + i * (mediaButtonSize + padding), startY),
        sf::Vector2f(mediaButtonSize, mediaButtonSize),
        font,
        symbols[i],
        16,
        [&, i]() { 
            if (i == 0) { simSpeed = 1.f; std::cout << "Simulation started (1x)\n"; }
            else if (i == 1) { simSpeed = 0.f; std::cout << "Simulation stopped\n"; }
            else if (i == 2) { simSpeed = 1.f; std::cout << "Simulation 2x speed\n"; }
            else if (i == 3) { simSpeed = 3.f; std::cout << "Simulation 3x speed\n"; }
        }
    ));
}



    // Main control buttons
    middlePanel.addButton(Button(
        sf::Vector2f(20.f, 60.f), sf::Vector2f(120.f, 40.f),
        font, "Randomize", 18,
        [&]() { life.randomize(); std::cout << "Randomized the grid.\n"; }
    ));

    middlePanel.addButton(Button(
        sf::Vector2f(20.f, 110.f), sf::Vector2f(120.f, 40.f),
        font, "Clear", 18,
        [&]() { life.clear(); std::cout << "Cleared the grid.\n"; }
    ));

    middlePanel.addButton(Button(
        sf::Vector2f(20.f, 160.f), sf::Vector2f(120.f, 40.f),
        font, "Update", 18,
        [&]() { life.update(); std::cout << "Updated the grid.\n"; }
    ));

    // Toric toggle buttons
middlePanel.addButton(Button(sf::Vector2f(20.f, 220.f), sf::Vector2f(120.f, 40.f), font, "Toric ON", 14,
    [&]() { life.setToric(true); std::cout << "Toric mode ON\n"; }));

middlePanel.addButton(Button(sf::Vector2f(20.f, 270.f), sf::Vector2f(120.f, 40.f), font, "Toric OFF", 14,
    [&]() { life.setToric(false); std::cout << "Toric mode OFF\n"; }));

    middlePanel.layoutButtons();

    // Right panel 1 buttons
    rightPanel1.addButton(Button(sf::Vector2f(20.f, 20.f), sf::Vector2f(83.f, 83.f), font, "Conway3D", 16,
        [&]() { life.setMode(LifeMode::Current3D); std::cout << "Ruleset is conway's game of life 3D.\n"; }));

    rightPanel1.addButton(Button(sf::Vector2f(20.f, 70.f), sf::Vector2f(83.f, 83.f), font, "Conway2D", 16,
        [&]() { life.setMode(LifeMode::Conway2D); std::cout << "Ruleset is conway's game of life 2D.\n"; }));

    rightPanel1.addButton(Button(sf::Vector2f(20.f, 120.f), sf::Vector2f(83.f, 83.f), font, "Custom3D", 16,
        [&]() { life.setMode(LifeMode::Custom3D); std::cout << "Ruleset is custom game of life 3D.\n"; }));

    rightPanel1.addButton(Button(sf::Vector2f(20.f, 170.f), sf::Vector2f(83.f, 83.f), font, "Custom2D", 16,
        [&]() { life.setMode(LifeMode::Custom2D); std::cout << "Ruleset is custom game of life 2D.\n"; }));

    rightPanel1.layoutButtons();

    // Right panel 2 buttons
    rightPanel2.addButton(Button(sf::Vector2f(20.f, 20.f), sf::Vector2f(83.f, 83.f), font, "Heatmap", 16,
        [&]() { heatmap.setPattern(ColoringPattern::Heatmap); std::cout << "Coloring scheme is heatmap.\n"; }));

    rightPanel2.addButton(Button(sf::Vector2f(20.f, 70.f), sf::Vector2f(83.f, 83.f), font, "GrayScale", 16,
        [&]() { heatmap.setPattern(ColoringPattern::Grayscale); std::cout << "Coloring scheme is grayscale.\n"; }));

    rightPanel2.addButton(Button(sf::Vector2f(20.f, 120.f), sf::Vector2f(83.f, 83.f), font, "ZFade", 16,
        [&]() { heatmap.setPattern(ColoringPattern::ZFade); std::cout << "Coloring scheme is z-fade.\n"; }));

    rightPanel2.addButton(Button(sf::Vector2f(20.f, 170.f), sf::Vector2f(83.f, 83.f), font, "BluePulse", 16,
        [&]() { heatmap.setPattern(ColoringPattern::BluePulse); std::cout << "Coloring scheme is blue pulse.\n"; }));

    rightPanel2.layoutButtons();

    // Camera
    Camera camera;

    // Current pattern name for logging
    std::string currentPatternName = "none"; // default folder

// Terminal command handling
std::atomic<bool> running{true};
int logStep = 0;
std::thread cmdThread([&]() {
    std::string line;
    while (running) {
        if (!std::getline(std::cin, line)) break;

        if (line == "clear") life.clear();
        else if (line == "random") life.randomize();
        else if (line == "update") life.update();
        else if (line.rfind("init ", 0) == 0) { // "init <PatternName>"
            std::string pattern = line.substr(5);
            currentPatternName = pattern;
            logStep = 0;
            std::cout << "Test loaded the file :" <<  life.loadFromFile("IO/" + pattern + "/initial.txt") << "\n";
            life.loadFromFile(pattern);
            std::cout << "Loaded pattern: " << pattern << " (logStep reset to 0)\n";
        }
        else if (line == "list") {
            auto configs = listStartingConfigs("IO");
            std::cout << "Available starting patterns:\n";
            for (const auto& c : configs)
                std::cout << " - " << c << "\n";
        }
        // Simulation controls
        else if (line == "stop") {
            simSpeed = 0.f;
            std::cout << "Simulation stopped\n";
        }
        else if (line == "start") {
            simSpeed = 1.f;
            std::cout << "Simulation started (1x)\n";
        }
        else if (line == "speed1") {
            simSpeed = 1.f;
            std::cout << "Simulation speed 2x\n";
        }
        else if (line == "speed2") {
            simSpeed = 3.f;
            std::cout << "Simulation speed 3x\n";
        }
        // Ruleset commands
        else if (line == "Conway3D") {
            life.setMode(LifeMode::Current3D);
            std::cout << "Ruleset set to Conway 3D.\n";
        }
        else if (line == "Conway2D") {
            life.setMode(LifeMode::Conway2D);
            std::cout << "Ruleset set to Conway 2D.\n";
        }
        else if (line == "Custom3D") {
            life.setMode(LifeMode::Custom3D);
            std::cout << "Ruleset set to Custom 3D.\n";
        }
        else if (line == "Custom2D") {
            life.setMode(LifeMode::Custom2D);
            std::cout << "Ruleset set to Custom 2D.\n";
        }
        // Coloring mode commands
        else if (line == "Heatmap") {
            heatmap.setPattern(ColoringPattern::Heatmap);
            std::cout << "Coloring set to Heatmap.\n";
        }
        else if (line == "GrayScale") {
            heatmap.setPattern(ColoringPattern::Grayscale);
            std::cout << "Coloring set to Grayscale.\n";
        }
        else if (line == "ZFade") {
            heatmap.setPattern(ColoringPattern::ZFade);
            std::cout << "Coloring set to ZFade.\n";
        }
        else if (line == "BluePulse") {
            heatmap.setPattern(ColoringPattern::BluePulse);
            std::cout << "Coloring set to BluePulse.\n";
        }
        else if (line == "help") {
            std::cout <<
                "Available commands:\n"
                "  clear         - Clear the entire grid.\n"
                "  random        - Randomize the grid.\n"
                "  update        - Perform one simulation step.\n"
                "  init <name>   - Load initial pattern from folder 'IO/<name>'.\n"
                "  list          - List all available initial patterns.\n"
                "  stop          - Pause the simulation.\n"
                "  start         - Start/resume simulation at 1x speed.\n"
                "  speed1        - Set simulation speed to 2x.\n"
                "  speed2        - Set simulation speed to 3x.\n"
                "  Conway3D      - Set ruleset to Conway 3D.\n"
                "  Conway2D      - Set ruleset to Conway 2D.\n"
                "  Custom3D      - Set ruleset to Custom 3D.\n"
                "  Custom2D      - Set ruleset to Custom 2D.\n"
                "  Heatmap       - Set coloring mode to Heatmap.\n"
                "  GrayScale     - Set coloring mode to Grayscale.\n"
                "  ZFade         - Set coloring mode to ZFade.\n"
                "  BluePulse     - Set coloring mode to BluePulse.\n"
                "  help          - Show this message.\n";
        }
        else {
            std::cout << "Unknown command: " << line << "\n";
        }
    }
});



    // Event loop
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (auto evOpt = window.pollEvent()) {
            const sf::Event& event = *evOpt;

            if (event.is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            if (auto kp = event.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape)
                    window.close();
                else if (kp->code == sf::Keyboard::Key::C)
                    life.clear();
                else if (kp->code == sf::Keyboard::Key::Space)
                    life.update();
            }

            if (auto mp = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mp->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mp->position.x),
                                         static_cast<float>(mp->position.y));
                    middlePanel.handleClick(mousePos);
                    rightPanel1.handleClick(mousePos);
                    rightPanel2.handleClick(mousePos);
                }
            }

            camera.handleEvent(event);
        }

        camera.update(deltaTime);

        // --- update & log ---
        if (updateClock.getElapsedTime().asSeconds() >= updateInterval / (simSpeed > 0 ? simSpeed : 1.f)) {
            if (simSpeed > 0.f) { // only update if not paused
                life.update();

                // Only save if a real pattern is loaded
                if (currentPatternName != "none") {
                    life.saveToFile(currentPatternName, logStep++);
                }
            }
            updateClock.restart();
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = static_cast<float>(window.getSize().x) / window.getSize().y;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspect);

        renderer.render(life, shader, view, projection);

        window.pushGLStates();
        middlePanel.render(window);
        rightPanel1.render(window);
        rightPanel2.render(window);
        window.popGLStates();

        window.display();
    }

    running = false;
    if (cmdThread.joinable()) cmdThread.join();

    std::cout << "Exiting application..." << std::endl;
    return 0;
}
