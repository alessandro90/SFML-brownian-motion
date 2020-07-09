#ifndef COORDIANTE2D_H
#define COORDIANTE2D_H

#include "SFML/Graphics.hpp"

sf::Vector2f operator+(sf::Vector2f const &p1, sf::Vector2f const &p2)
{
    return {p1.x + p2.x, p1.y + p2.y};
}

sf::Vector2f operator-(sf::Vector2f const &p1, sf::Vector2f const &p2)
{
    return {p1.x - p2.x, p1.y - p2.y};
}

sf::Vector2f operator*(sf::Vector2f const &p, float n)
{
    return {p.x * n, p.y * n};
}

sf::Vector2f operator*(float n, sf::Vector2f const &p)
{
    return p * n;
}

#endif