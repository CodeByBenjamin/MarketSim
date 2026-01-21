#include <iomanip>
#include <sstream>
#include <string>
#include <ios>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include "UIHelpers.h"

std::string UIHelper::formatPrice(double price)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << price;
    return ss.str();
}

void UIHelper::drawLabel(sf::RenderTarget& target, const sf::Font& font, const std::string& label, int fontSize, float x, float y, TextSnap snap, float offset, sf::Color color)
{
    sf::Text text(font);
    text.setCharacterSize(fontSize);
    text.setFillColor(color);

    std::string rowStr = label;
    text.setString(rowStr);

    float fullOffset = offset;

    switch (snap)
    {
    case LEFT:
        break;
    case CENTER:
        fullOffset -= text.getLocalBounds().size.x / 2.f;
        break;
    case RIGHT:
        fullOffset -= text.getLocalBounds().size.x;
        break;
    }

    text.setPosition({ x + fullOffset, y });

    target.draw(text);
}

void UIHelper::drawColoredRect(sf::RenderTarget& target, float x, float y, float width, float height, TextSnap snap, float offset, sf::Color color)
{
    sf::RectangleShape rect;

    rect.setSize(sf::Vector2f(width, height));
    rect.setFillColor(color);

    float fullOffset = offset;

    switch (snap)
    {
    case LEFT:
        break;
    case CENTER:
        fullOffset -= width / 2.f;
        break;
    case RIGHT:
        fullOffset -= width;
        break;
    }

    rect.setPosition({ x + fullOffset, y });

    target.draw(rect);
}