#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#include "lob.h"

namespace Theme {
    const sf::Color Background = sf::Color(23, 27, 36);   // Background color
    const sf::Color Surface = sf::Color(43, 43, 64);    // Panel backgrounds
    const sf::Color Border = sf::Color(47, 51, 61);    // Dividers

    const sf::Color TextMain = sf::Color(242, 242, 242); // Primary numbers
    const sf::Color TextDim = sf::Color(139, 148, 158); // Timestamps/Labels

    const sf::Color Bid = sf::Color(72, 168, 100);  // Your Green
    const sf::Color Ask = sf::Color(188, 72, 72);   // Your Red
    const sf::Color BidBG = sf::Color(72, 168, 100, 80);  // Your Green
    const sf::Color AskBG = sf::Color(188, 72, 72, 80);   // Your Red
    const sf::Color Accent = sf::Color(210, 153, 34);  // "Special" info
}

enum TextMargin {
    LEFT,
    CENTER,
    RIGHT
};

void drawDefault(sf::RenderWindow& window, sf::Font& font, float lobWidth)
{
    //Draw LOBPanel

    sf::RectangleShape lobPanel;

    lobPanel.setSize(sf::Vector2f(lobWidth, 1080.f));
    lobPanel.setFillColor(Theme::Surface);
    lobPanel.setPosition({ 0.f, 0.f });

    window.draw(lobPanel);

    //Draw Label Text

    sf::Text text(font);
    text.setCharacterSize(22);
    text.setStyle(sf::Text::Bold);

    text.setFillColor(Theme::Bid);
    std::string rowStr = "BIDS";
    text.setString(rowStr);
    text.setPosition({ lobWidth / 4 - text.getLocalBounds().size.x / 2, 20.f });

    window.draw(text);

    text.setFillColor(Theme::Ask);
    rowStr = "ASKS";
    text.setString(rowStr);
    text.setPosition({ 3 * lobWidth / 4 - text.getLocalBounds().size.x / 2, 20.f });

    window.draw(text);
}

std::string formatPrice(double price)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << price;
    return ss.str() + " $";
}

void drawLabel(sf::RenderWindow& window, sf::Font font, std::string label, float width, float height, TextMargin margin, sf::Color color)
{
    sf::Text text(font);
    text.setCharacterSize(22);
    text.setFillColor(color);

    std::string rowStr = label;
    text.setString(rowStr);

    float offset = 0.f;

    switch (margin)
    {
    case LEFT:
        offset = 0.f;
        break;
    case CENTER:
        offset = text.getLocalBounds().size.x / 2;
        break;
    case RIGHT:
        offset = text.getLocalBounds().size.x;
        break;
    }

    text.setPosition({ width - offset, height });

    window.draw(text);
}

void drawColoredRect(sf::RenderWindow& window, float x, float y, float width, float height, sf::Color color)
{
    sf::RectangleShape rect;

    rect.setSize(sf::Vector2f(width, height));
    rect.setFillColor(color);
    rect.setPosition({ x, y });

    window.draw(rect);
}

void drawLOB(sf::RenderWindow& window, sf::Font& font, float lobWidth, LimitOrderBook& LOB)
{
    Side side = BUY;

    if (side == BUY)
    {
        float height = 70;

        double price = 3.00;
        long volume = 5;
        for (int i = 0; i < 10; i++)
        {
            drawColoredRect(window, 0, height + 35 * i, lobWidth / 2, 30, Theme::BidBG);
            drawLabel(window, font, formatPrice(price), lobWidth / 2 - 10.f, height + 35 * i, RIGHT, Theme::Bid);
            drawLabel(window, font, std::to_string(volume), 0, height + 35 * i, LEFT, Theme::Bid);
        }

        height = 70;

        price = 8.00;
        volume = 10;
        for (int i = 0; i < 15; i++)
        {
            drawColoredRect(window, lobWidth / 2, height + 35 * i, lobWidth / 2, 30, Theme::AskBG);
            drawLabel(window, font, formatPrice(price), lobWidth / 2 + 10.f, height + 35 * i, LEFT, Theme::Ask);
            drawLabel(window, font, std::to_string(volume), lobWidth - 10.f, height + 35 * i, RIGHT, Theme::Ask);
        }
    }
    else
    {
        //sell side
    }

    drawColoredRect(window, lobWidth, 0.f, 10.f, 1080.f, Theme::Border);
    drawColoredRect(window, lobWidth / 2, 0.f, 10.f, 1080.f, Theme::Border);
}

void runLOBTests(LimitOrderBook& lob)
{
    std::cout << "--- Submitting Orders to LOB ---" << std::endl;

    std::vector<Order> testOrders = {
        {0, 101, 10.00, 50, Side::BUY, 0},
        {0, 102, 10.05, 50, Side::SELL, 0},
        {0, 103, 10.00, 30, Side::SELL, 0}
    };

    for (const auto& order : testOrders)
    {
        Order submittedOrder = order;
        lob.processOrder(submittedOrder);
    }

    LOBState state = lob.getLOBState();
    std::cout << "\n--- Final LOB State ---" << std::endl;
    std::cout << "Best Bid: " << state.bestBidPrice << " Vol: " << state.bestBidVolume << std::endl;
    std::cout << "Best Ask: " << state.bestAskPrice << " Vol: " << state.bestAskVolume << std::endl;
    std::cout << "Trades Executed: " << lob.getTradeHistory().size() << std::endl;
    std::cout << "Last Trade Price: " << lob.getLastTradePrice() << std::endl;

    std::cout << "\n--- Trade History ---" << std::endl;
    for (const auto& trade : lob.getTradeHistory())
    {
        std::cout << "ID: " << trade.tradeId
            << " | Time: " << trade.timeStamp
            << " | Price: " << trade.price
            << " | Volume: " << trade.volume
            << " | Buyer Order ID: " << trade.buyerOrderId
            << " | Seller Order ID: " << trade.sellerOrderId << std::endl;
    }
}

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Market simulator");
    window.setFramerateLimit(144);

    sf::Font font;
    if (!font.openFromFile("fonts/RobotoMono-Regular.ttf"))
    {
        std::cout << "Error loading font!" << std::endl;
    }

    LimitOrderBook LOB;
    //runLOBTests(LOB);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear(Theme::Background);

        float lobWidth = window.getSize().x * 0.3f;

        drawDefault(window, font, lobWidth);
        drawLOB(window, font, lobWidth, LOB);
        //drawRow(window, font, "");
        window.display();
    }
}