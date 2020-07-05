#ifndef POTENTIAL_H
#define POTENTIAL_H

#include "pair.hpp"

struct Potential
{
    virtual ~Potential() = default;
    virtual Pair gradient(Pair const &) const = 0;
};

struct Flat: Potential
{
    Pair gradient(Pair const &point) const override
    {
        (void)point;
        return {0.f, 0.f};
    }
};

#endif