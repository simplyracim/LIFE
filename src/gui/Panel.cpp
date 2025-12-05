#include "gui/Panel.h"

Panel::Panel(const sf::Vector2f& pos,
             const sf::Vector2f& size,
             const sf::Font& font,
             const std::string& title)
             : m_title(font)
{
    // Panel background
    m_shape.setPosition(pos);
    m_shape.setSize(size);
    m_shape.setFillColor(sf::Color(40, 40, 40, 200));
    m_shape.setOutlineThickness(2.f);
    m_shape.setOutlineColor(sf::Color::White);

    // Title
    m_title.setCharacterSize(20);
    m_title.setFillColor(sf::Color::White);

    // Title position
    m_title.setPosition({pos.x + 10.f, pos.y + 5.f});
}

void Panel::addButton(const Button& btn) {
    // Make a copy so we can modify its position
    Button b = btn;

    // Position relative to the panel
    sf::Vector2f panelPos = m_shape.getPosition();
    b.setPosition(btn.getPosition() + panelPos);

    // Add to the panel's button list
    m_buttons.push_back(std::move(b));
}


void Panel::handleEvent(const sf::Event& event, const sf::RenderWindow&) {
    // Nothing — Panel doesn't react to keyboard
}

bool Panel::contains(const sf::Vector2f& point) const {
    return m_shape.getGlobalBounds().contains(point);
}

void Panel::handleClick(const sf::Vector2f& mousePos) {
    if (!contains(mousePos))
        return;

    for (auto& btn : m_buttons)
        btn.handleClick(mousePos);
}

void Panel::render(sf::RenderWindow& window) {
    window.draw(m_shape);
    window.draw(m_title);

    for (auto& btn : m_buttons)
        btn.render(window);
}

void Panel::layoutButtons() {
    float padding = 10.f; // space between buttons
    float x = padding;
    float y = padding;
    float maxRowHeight = 0.f;

    sf::Vector2f panelPos = m_shape.getPosition();
    sf::Vector2f panelSize = m_shape.getSize();

    for (auto& button : m_buttons) {
        sf::Vector2f size = button.getSize();

        // If button exceeds panel width, move to next row
        if (x + size.x + padding > panelSize.x) {
            x = padding;
            y += maxRowHeight + padding;
            maxRowHeight = 0.f;
        }

        button.setPosition(panelPos + sf::Vector2f(x, y));
        x += size.x + padding;
        if (size.y > maxRowHeight) maxRowHeight = size.y;
    }
}
