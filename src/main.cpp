#include <iostream>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Window/Event.hpp>

#include "Clock.h"
#include "datatypes.h"
#include "UIHelpers.h"
#include "LimitOrderBook.h"
#include "LOBPanel.h"
#include "DepthChart.h"
#include "Trader.h"

static void runLOBTests(LimitOrderBook& LOB)
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
        LOB.processOrder(submittedOrder);
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

    Clock clock;

    double dt = 1.0; //Ticks per update

    double updatesPerSecond = 10.0;
    double realDt = 1.0 / updatesPerSecond;
    auto lastTime = std::chrono::high_resolution_clock::now();

    LOBPanel lobPanel;

    LimitOrderBook LOB;
    DepthChart depthChart;
    runLOBTests(LOB);

    float lobWidth = static_cast<float>(window.getSize().x * 0.25f);
    float chartWidth = static_cast<float>(window.getSize().x * 0.25f);
    float chartHeight = static_cast<float>(window.getSize().y * 0.25f);

    bool lobDirty = true;

    Trader traders[10];

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
            }
        }

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - lastTime;

        while (elapsed.count() >= realDt)
        {
            clock.advance(dt);
            lastTime += std::chrono::duration_cast<
                std::chrono::high_resolution_clock::duration>(
                    std::chrono::duration<double>(realDt)
                );
            elapsed = now - lastTime;

            LOB.update();
            for (size_t i = 0; i < 10; i++)
            {
                traders[i].update(LOB, clock.now());
            }

            lobDirty = true;
        }

        window.clear(Theme::Background);

        if (lobDirty)
        {
            depthChart.update(LOB, chartWidth, chartHeight, window.getSize());
            lobDirty = false;
        }

        lobPanel.draw(window, font, LOB, lobWidth);
        window.draw(depthChart);
        window.display();
    }
}