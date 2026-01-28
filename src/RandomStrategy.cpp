#include <cmath>
#include <random>

#include "RandomStrategy.h"
#include "Trader.h"
#include "LimitOrderBook.h"
#include "Clock.h"

void RandomStrategy::decide(Trader& trader, LimitOrderBook& LOB, Clock& clock) {
    static std::mt19937 rng(std::random_device{}());

    double perceivedValue = 20.0; // The "True" value
    double marketPrice = LOB.getMidPriceHistory().back();

    double mid = (marketPrice * 0.7) + (perceivedValue * 0.3);

    for (long id : trader.getActiveOrderIds()) {
        LOB.cancelOrder(id);
    }
    trader.clearActiveOrderIds();

    std::uniform_real_distribution<double> distDist(0.0005, 0.005); // 0.05% to 0.50%
    std::uniform_int_distribution<long> volDist(5, 20);

    double myOffset = mid * distDist(rng);

    std::uniform_real_distribution<double> jitter(-0.0005, 0.0005);
    double myRefPrice = mid * (1.0 + jitter(rng));

    Order bid = { 0, trader.getId(), myRefPrice - myOffset, volDist(rng), Side::BUY, clock.now() };
    if (bid.price < 0.01) bid.price = 0.01;
    trader.addActiveOrderId(LOB.processOrder(bid, clock));

    Order ask = { 0, trader.getId(), myRefPrice + myOffset, volDist(rng), Side::SELL, clock.now() };
    if (ask.price < 0.01) ask.price = 0.01;
    trader.addActiveOrderId(LOB.processOrder(ask, clock));
}