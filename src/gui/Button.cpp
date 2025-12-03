#include "gui/Button.h"

Button::Button(const sf::Vector2f& pos, const sf::Vector2f& size,
               const sf::Font& font, const std::string& textStr,
               unsigned int charSize, std::function<void()> callback)
    : m_text(font), // SFML 3 requires font in constructor
      m_onClick(callback)
{
    m_shape.setPosition(pos);
    m_shape.setSize(size);
    m_shape.setFillColor(sf::Color(70, 130, 180)); // steel blue

    m_text.setString(textStr);
    m_text.setCharacterSize(charSize);
    m_text.setFillColor(sf::Color::White);

    // Center the text
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin({0, 0});
    m_text.setPosition(pos + size / 2.f);
}

void Button::draw(sf::RenderTarget& target) {
    target.draw(m_shape);
    target.draw(m_text);
}

void Button::handleClick(const sf::Vector2f& mousePos) {
    if (m_shape.getGlobalBounds().contains(mousePos)) {
        if (m_onClick) m_onClick();
    }
}
