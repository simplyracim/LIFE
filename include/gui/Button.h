#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button {
public:
    Button(const sf::Vector2f& pos, const sf::Vector2f& size,
           const sf::Font& font, const std::string& textStr,
           unsigned int charSize, std::function<void()> callback);

    void draw(sf::RenderTarget& target);
    void handleClick(const sf::Vector2f& mousePos);

private:
    sf::RectangleShape m_shape;
    sf::Text m_text;
    std::function<void()> m_onClick;
};
