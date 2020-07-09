#ifndef CIRCULAR_ARRAY_H
#define CIRCULAR_ARRAY_H

#include <vector>
#include <cstddef>


template<typename T, std::size_t N>
class CircularArray
{
public:
    CircularArray()
    {
        container.reserve(N);
    }
    void push_back(T const &t)
    {
        if (container.size() < N)
        {
            container.push_back(t);
            last = container.size() - 1;
        }
        else
        {
            container[first] = t;
            last = first;
            first = (first + 1) % N;
            current = first;
        }
    }
    bool hasNext() const
    {
        return counter < container.size();
    }
    T const &getNext()
    {
        auto const &i = container[current];
        current = (current + 1) % N;
        ++counter;
        return i;
    }
    void reload()
    {
        current = first;
        counter = 0;
    }
    auto size() const
    {
        return container.size();
    }
    auto const &operator[](std::size_t i) const
    {
        return container[(first + i) % N];
    }
    auto &operator[](std::size_t i)
    {
        return container[(first + i) % N];
    }
private:
    std::size_t counter{};
    std::size_t first{}, last{N}, current{};
    std::vector<T> container;
};
#endif