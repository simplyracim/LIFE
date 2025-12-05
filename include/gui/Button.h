#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include "Widget.h"

class Button : public Widget {
public:
    Button(const sf::Vector2f& pos, const sf::Vector2f& size,
           const sf::Font& font, const std::string& textStr,
           unsigned int charSize, std::function<void()> callback);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    bool contains(const sf::Vector2f& point) const override;

    void handleClick(const sf::Vector2f& mousePos);

    // setters and getters
    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;

private:
    sf::RectangleShape m_shape;
    sf::Text m_text;
    std::function<void()> m_onClick;
};
