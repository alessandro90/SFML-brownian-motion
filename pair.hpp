#ifndef PAIR_H
#define PAIR_H

struct Pair
{
    float x, y;
};

Pair operator+(Pair const &p1, Pair const &p2)
{
    return {p1.x + p2.x, p1.y + p2.y};
}

Pair operator-(Pair const &p1, Pair const &p2)
{
    return {p1.x - p2.x, p1.y - p2.y};
}

Pair operator*(Pair const &p, float n)
{
    return {p.x * n, p.y * n};
}

Pair operator*(float n, Pair const &p)
{
    return p * n;
}

#endif