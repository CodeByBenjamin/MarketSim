#pragma once

namespace sf {
    class RenderWindow;
    class Font;
}

#include "LimitOrderBook.h"

class LOBPanel
{
private:
public:
	void draw(sf::RenderWindow& window, const sf::Font& font, const LimitOrderBook& LOB, float lobWidth);
};