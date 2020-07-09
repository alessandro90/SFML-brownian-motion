#ifndef NOISE_H
#define NOISE_H

#include <random>

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

#endif