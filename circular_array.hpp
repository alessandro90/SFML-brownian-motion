#ifndef CIRCULAR_ARRAY_H
#define CIRCULAR_ARRAY_H

#include <array>
#include <cstddef>

template <typename T, std::size_t N>
class CircularArray
{
public:
    constexpr void push_back(T const &t)
    {
        if (sizeCounter < N)
        {
            container[sizeCounter] = t;
            ++sizeCounter;
            last = sizeCounter - 1;
        }
        else
        {
            container[first] = t;
            last = first;
            first = (first + 1) % N;
            current = first;
        }
    }
    constexpr bool hasNext() const
    {
        return counter < sizeCounter;
    }
    constexpr T const &getNext()
    {
        auto const &i = container[current];
        current = (current + 1) % N;
        ++counter;
        return i;
    }
    constexpr void reload()
    {
        current = first;
        counter = 0;
    }
    constexpr auto size() const
    {
        return sizeCounter;
    }
    constexpr auto const &operator[](std::size_t i) const
    {
        return container[(first + i) % N];
    }
    constexpr auto &operator[](std::size_t i)
    {
        return container[(first + i) % N];
    }

private:
    std::size_t counter{};
    std::size_t sizeCounter{};
    std::size_t first{}, last{}, current{};
    std::array<T, N> container{};
};
#endif