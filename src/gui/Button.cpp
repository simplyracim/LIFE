#include "gui/Button.h"

Button::Button(const sf::Vector2f& pos, const sf::Vector2f& size,
               const sf::Font& font, const std::string& textStr,
               unsigned int charSize, std::function<void()> callback)
    : m_text(font), m_onClick(callback)
{
    m_shape.setPosition(pos);
    m_shape.setSize(size);
    m_shape.setFillColor(sf::Color(70, 130, 180));

    m_text.setFont(font);
    m_text.setString(textStr);
    m_text.setCharacterSize(charSize);
    m_text.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
    m_text.setPosition(pos + size / 2.f);
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow&) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseButton = event.getIf<sf::Event::MouseButtonPressed>();
        sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x),
                              static_cast<float>(mouseButton->position.y));
        handleClick(mousePos);
    }
}

void Button::update(float dt) { (void)dt; }

void Button::render(sf::RenderWindow& window) {
    window.draw(m_shape);
    window.draw(m_text);
}

bool Button::contains(const sf::Vector2f& point) const {
    return m_shape.getGlobalBounds().contains(point);
}

void Button::handleClick(const sf::Vector2f& mousePos) {
    if (contains(mousePos) && m_onClick) m_onClick();
}

sf::Vector2f Button::getSize() const {
    return m_shape.getSize();
}

void Button::setPosition(const sf::Vector2f& pos) {
    m_shape.setPosition(pos);
    m_text.setPosition(pos + m_shape.getSize() / 2.f);
}

sf::Vector2f Button::getPosition() const {
    return m_shape.getPosition();
}
