#pragma once

#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>

enum Side
{
	BUY,
	SELL
};

struct Order
{
	long id;
	long agentId;
	double price;
	long volume;
	Side side;
	long long timeStamp;
};

struct TradeRecord
{
	long tradeId;
	double price;
	long volume;
	long buyerOrderId;
	long sellerOrderId;
	long long timeStamp;
};

struct LOBState
{
	double bestBidPrice;
	long bestBidVolume;
	double bestAskPrice;
	long bestAskVolume;
};

struct DepthPoint {
	float price;
	long totalVolume;
};