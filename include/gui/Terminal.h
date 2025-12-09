#pragma once
#include "Widget.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <deque>

class Terminal : public Widget {
public:
    Terminal(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Font& font);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    bool contains(const sf::Vector2f& point) const override;

    // Add a line to the terminal output
    void print(const std::string& line);

    // Set a callback function that will be called when user enters a command
    void setCommandCallback(std::function<void(const std::string&)> callback);

private:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    const sf::Font& m_font;

    std::deque<std::string> m_outputLines; // store previous lines
    std::string m_inputLine;               // current input
    sf::Text m_inputText;

    unsigned int m_maxLines;

    std::function<void(const std::string&)> m_commandCallback;

    void executeCommand(const std::string& cmd);
};
