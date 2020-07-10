#ifndef NOISE_H
#define NOISE_H

#include <random>
#include <cstdint>
#include "SFML/Graphics.hpp"

class Noise
{
public:
    float get()
    {
        return 50. * noise(gen);
    }

private:
    static inline std::mt19937 gen{std::random_device{}()};
    std::normal_distribution<float> noise{0., 1.};
};

class ColorPicker
{
public:
    sf::Color pick()
    {
        return {dist(gen), dist(gen), dist(gen)};
    }

private:
    static inline std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<std::uint8_t> dist{0, 255};
};

#endif