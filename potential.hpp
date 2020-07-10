#ifndef POTENTIAL_H
#define POTENTIAL_H

#include "coordinate2d.hpp"
#include <cmath>

struct Potential
{
    virtual ~Potential() = default;
    virtual sf::Vector2f gradient(sf::Vector2f const &point) const = 0;
    virtual void setSize(sf::Vector2u const &size) = 0;
};

struct Flat : Potential
{
    void setSize(sf::Vector2u const &size) override { (void)size; }
    sf::Vector2f gradient(sf::Vector2f const &point) const override
    {
        (void)point;
        return {0.f, 0.f};
    }
};

struct SingleWell : Potential
{
    explicit SingleWell(sf::Vector2u const &size,
                        float a = 1.f,
                        float b = 1.f,
                        float c = 0.f,
                        float d = 0.f) : a{a}, b{b}, c{c}, d{d}
    {
        this->size.x = static_cast<float>(size.x) / 2.f;
        this->size.y = static_cast<float>(size.y) / 2.f;
    }
    void setSize(sf::Vector2u const &size) override
    {
        this->size.x = static_cast<float>(size.x) / 2.f;
        this->size.y = static_cast<float>(size.y) / 2.f;
    }
    sf::Vector2f gradient(sf::Vector2f const &point) const override
    {
        return {
            2.f * a * (point.x - size.x) + c * point.x,
            2.f * b * (point.y - size.y) + d * point.y};
    }

private:
    float const a, b, c, d;
    sf::Vector2f size;
};

struct InverseSingleWell : Potential
{
    explicit InverseSingleWell(sf::Vector2u const &size,
                               float a = 1.f,
                               float b = 1.f,
                               float c = 0.f,
                               float d = 0.f) : singleWell{size, a, b, c, d} {}
    void setSize(sf::Vector2u const &size) override
    {
        singleWell.setSize(size);
    }
    sf::Vector2f gradient(sf::Vector2f const &point) const override
    {
        return -singleWell.gradient(point);
    }

private:
    SingleWell singleWell;
};

struct CosineSum : Potential
{
    explicit CosineSum(sf::Vector2u const &size) : size{static_cast<float>(size.x),
                                                        static_cast<float>(size.y)} {}
    void setSize(sf::Vector2u const &size) override
    {
        this->size.x = static_cast<float>(size.x) / 2.f;
        this->size.y = static_cast<float>(size.y) / 2.f;
    }
    sf::Vector2f gradient(sf::Vector2f const &point) const override
    {
        return {-10.f * std::sin(0.05f * (point.x + size.x)),
                -10.f * std::sin(0.05f * (point.y + size.y))};
    }

private:
    sf::Vector2f size;
};

#endif