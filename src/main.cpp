#include <iostream>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Window/Event.hpp>

#include "datatypes.h"
#include "UIHelpers.h"
#include "LimitOrderBook.h"
#include "DepthChart.h"

static void runLOBTests(LimitOrderBook& lob)
{
    std::vector<Order> testOrders = {
        {0, 101, 10.00, 150, Side::BUY, 0}, 
        {0, 102, 9.95,  80,  Side::BUY, 0},
        {0, 103, 9.90,  200, Side::BUY, 0}, 
        {0, 104, 9.85,  40,  Side::BUY, 0},
        {0, 105, 10.00, 50,  Side::BUY, 0}, 

        {0, 201, 10.10, 100, Side::SELL, 0},
        {0, 202, 10.15, 60,  Side::SELL, 0},
        {0, 203, 10.20, 300, Side::SELL, 0},
        {0, 204, 10.25, 20,  Side::SELL, 0},
        {0, 205, 10.10, 50,  Side::SELL, 0},

        {0, 301, 10.10, 30,  Side::BUY, 0}
    };

    for (const auto& order : testOrders)
    {
        Order submittedOrder = order;
        lob.processOrder(submittedOrder);
    }
}

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Market simulator", sf::State::Fullscreen);
    window.setFramerateLimit(144);

    sf::Font font;
    if (!font.openFromFile("fonts/RobotoMono-Regular.ttf"))
    {
        std::cout << "Error loading font!" << std::endl;
    }

    LimitOrderBook LOB;
    DepthChart depthChart;
    runLOBTests(LOB);

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

        float lobWidth = window.getSize().x * 0.25f;
        float chartWidth = window.getSize().x * 0.25f;
        float chartHeight = window.getSize().y * 0.25f;

        depthChart.update(LOB, chartWidth, chartHeight, window.getSize());

        LOB.draw(window, font, lobWidth);
        window.draw(depthChart);
        window.display();
    }
}