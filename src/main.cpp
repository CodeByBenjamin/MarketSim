#include <SFML/Graphics.hpp>
#include <iostream>

#include "lob.h"

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
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    LimitOrderBook LOB;

    runLOBTests(LOB);

    /*while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.display();
    }*/
}
