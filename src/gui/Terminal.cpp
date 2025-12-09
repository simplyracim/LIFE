#include "gui/Terminal.h"

Terminal::Terminal(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Font& font)
    : m_position(pos), m_size(size), m_font(font), m_maxLines(100), m_inputText(font)
{
    m_inputText.setFont(m_font);
    m_inputText.setCharacterSize(16);
    m_inputText.setFillColor(sf::Color::Green);
    m_inputText.setPosition({(float)m_position.x + 5, (float)m_position.y + m_size.y - 20});
}

void Terminal::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (!contains(sf::Vector2f(sf::Mouse::getPosition(window))))
        return;

    if (event.is<sf::Event::TextEntered>()) {
        auto* te = event.getIf<sf::Event::TextEntered>();
        if (!te) return;

        char c = static_cast<char>(te->unicode);

        if (c == '\b') { // backspace
            if (!m_inputLine.empty()) m_inputLine.pop_back();
        } else if (c == '\r' || c == '\n') { // enter
            executeCommand(m_inputLine);
            m_inputLine.clear();
        } else if (c >= 32 && c <= 126) { // printable
            m_inputLine += c;
        }
    }
}

void Terminal::update(float dt)
{
    (void)dt; // no dynamic update needed for now
}

void Terminal::render(sf::RenderWindow& window)
{
    // Draw background
    sf::RectangleShape bg(m_size);
    bg.setPosition(m_position);
    bg.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(bg);

    // Draw output lines
    float y = m_position.y + 5;
    unsigned int startLine = m_outputLines.size() > m_maxLines ? m_outputLines.size() - m_maxLines : 0;
    for (unsigned int i = startLine; i < m_outputLines.size(); ++i) {
        sf::Text text(m_font);           // default constructor
        text.setFont(m_font);      // set font
        text.setCharacterSize(16); // set size
        text.setFillColor(sf::Color::White);
        text.setString(m_outputLines[i]);  // set the actual text string
        text.setPosition({m_position.x + 5, y});
        window.draw(text);
        y += 18;
    }

    // Draw input line
    m_inputText.setString("> " + m_inputLine);
    window.draw(m_inputText);
}


bool Terminal::contains(const sf::Vector2f& point) const
{
    return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
           point.y >= m_position.y && point.y <= m_position.y + m_size.y;
}

void Terminal::print(const std::string& line)
{
    m_outputLines.push_back(line);
    if (m_outputLines.size() > m_maxLines)
        m_outputLines.pop_front();
}

void Terminal::setCommandCallback(std::function<void(const std::string&)> callback)
{
    m_commandCallback = callback;
}

void Terminal::executeCommand(const std::string& cmd)
{
    print("> " + cmd);
    if (m_commandCallback)
        m_commandCallback(cmd);
}
