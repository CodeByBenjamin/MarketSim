#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

class LimitOrderBook;

class DepthChart : public sf::Drawable {
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::VertexArray bidTriangles;
    sf::VertexArray askTriangles;
public:
    DepthChart();

    void update(const LimitOrderBook& LOB, float chartWidth, float chartHeight, sf::Vector2u winSize);
};