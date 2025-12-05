#pragma once
#include <SFML/Graphics.hpp>

class Widget {
public:
    virtual ~Widget() = default;

    virtual void handleEvent(const sf::Event& event, const sf::RenderWindow& window) = 0;
    virtual void update(float dt) { (void)dt; }
    virtual void render(sf::RenderWindow& window) = 0;

    virtual bool contains(const sf::Vector2f& point) const = 0;
};
