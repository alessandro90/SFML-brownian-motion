#ifndef COORDIANTE2D_H
#define COORDIANTE2D_H

struct Coordinate2D
{
    float x, y;
};

Coordinate2D operator+(Coordinate2D const &p1, Coordinate2D const &p2)
{
    return {p1.x + p2.x, p1.y + p2.y};
}

Coordinate2D operator-(Coordinate2D const &p1, Coordinate2D const &p2)
{
    return {p1.x - p2.x, p1.y - p2.y};
}

Coordinate2D operator*(Coordinate2D const &p, float n)
{
    return {p.x * n, p.y * n};
}

Coordinate2D operator*(float n, Coordinate2D const &p)
{
    return p * n;
}

#endif