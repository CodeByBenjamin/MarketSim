#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

#include "datatypes.h"
#include "LimitOrderBook.h"
#include "DepthChart.h"
#include "UIHelpers.h"

DepthChart::DepthChart() {
    bidTriangles.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    askTriangles.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
}

void DepthChart::update(const LimitOrderBook& LOB, float chartWidth, float chartHeight, sf::Vector2u winSize) {
    long totalVolume = 0;
    std::vector<DepthPoint> depthPoints = LOB.depthChartPoints(0.5f, &totalVolume);

    if (depthPoints.empty()) return;

    float binWidth = chartWidth / static_cast<float>(depthPoints.size() - 1);
    float bottomOfChart = (float)winSize.y;
    float startX = winSize.x - chartWidth;

    size_t splitIndex = 0;
    for (size_t i = 0; i < depthPoints.size(); i++) {
        if (depthPoints[i].totalVolume == 0) {
            splitIndex = i;
            break;
        }
    }

    bidTriangles.resize((splitIndex + 1) * 2);
    size_t askCount = depthPoints.size() - splitIndex;
    askTriangles.resize(askCount * 2);

    for (size_t i = 0; i <= splitIndex; i++)
    {
        auto& curPoint = depthPoints[i];

        float hPerc = (totalVolume > 0) ? (float)curPoint.totalVolume / totalVolume : 0.f;
        float xPos = startX + (i * binWidth);
        float yPeak = bottomOfChart - (hPerc * chartHeight);

        bidTriangles[2 * i].position = { xPos, yPeak };
        bidTriangles[2 * i + 1].position = { xPos, bottomOfChart };

        sf::Color topCol = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::Bid;
        sf::Color botCol = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::BidBG;

        bidTriangles[2 * i].color = topCol;
        bidTriangles[2 * i + 1].color = botCol;
    }

    for (size_t i = 0; i < askCount; i++)
    {
        size_t dataIdx = splitIndex + i; //Offset to other side
        auto& curPoint = depthPoints[dataIdx];

        float hPerc = (totalVolume > 0) ? (float)curPoint.totalVolume / totalVolume : 0.f;
        float xPos = startX + (dataIdx * binWidth);
        float yPeak = bottomOfChart - (hPerc * chartHeight);

        askTriangles[2 * i].position = { xPos, yPeak };
        askTriangles[2 * i + 1].position = { xPos, bottomOfChart };

        sf::Color topCol = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::Ask;
        sf::Color botCol = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::AskBG;

        askTriangles[2 * i].color = topCol;
        askTriangles[2 * i + 1].color = botCol;
    }
}

void DepthChart::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(bidTriangles, states);
    target.draw(askTriangles, states);
}