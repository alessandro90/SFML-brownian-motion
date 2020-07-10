#ifndef NOISE_H
#define NOISE_H

#include <random>
#include <cstdint>
#include "SFML/Graphics.hpp"

template <template <typename> typename Distribution, typename T, std::size_t MULT = 1>
class Random
{
public:
    Random(T minV, T maxV) : dis{minV, maxV} {}
    T get()
    {
        return static_cast<T>(MULT) * dis(gen);
    }

private:
    static inline std::mt19937 gen{std::random_device{}()};
    Distribution<T> dis;
};

#endif