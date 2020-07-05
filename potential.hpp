#ifndef POTENTIAL_H
#define POTENTIAL_H

#include "coordinate2d.hpp"

struct Potential
{
    virtual ~Potential() = default;
    virtual Coordinate2D gradient(Coordinate2D const &point) const = 0;
};

struct Flat: Potential
{
    Coordinate2D gradient(Coordinate2D const &point) const override
    {
        (void)point;
        return {0.f, 0.f};
    }
};

struct SingleWell: Potential
{
    float const a, b, c, d;
    SingleWell(float a = 0.f, float b = 0.f, float c = 0.f, float d = 0.f):
    a{a}, b{b}, c{c}, d{d} {}
    
    Coordinate2D gradient(Coordinate2D const &point) const override
    {
        return {2.f * a * point.x, 2.f * b * point.y};
    }
};

#endif