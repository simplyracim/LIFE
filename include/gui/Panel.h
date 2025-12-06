#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Button.h"
#include "Widget.h"

class Panel : public Widget {
public:
    Panel(const sf::Vector2f& pos,
          const sf::Vector2f& size,
          const sf::Font& font,
          const std::string& title);

    void addButton(const Button& btn);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
    bool contains(const sf::Vector2f& point) const override;
    void handleClick(const sf::Vector2f& mousePos);
    void layoutButtons();

    // Accessors needed for main.cpp
    sf::Vector2f getSize() const { return m_shape.getSize(); }
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }

private:
    sf::RectangleShape m_shape;
    sf::Text m_title;

    std::vector<Button> m_buttons;
};
