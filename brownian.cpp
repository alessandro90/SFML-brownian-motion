#include "SFML/Graphics.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <utility>
#include <iostream>

using TimePoint = std::chrono::system_clock::time_point;

auto getElapsed(TimePoint const &start, TimePoint const &end)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

struct Potential
{
    virtual ~Potential() = default;
    virtual std::pair<float, float> gradient(std::pair<float, float> const &) const = 0;
};

struct Flat: Potential
{
    std::pair<float, float> gradient(std::pair<float, float> const &point) const override
    {
        (void)point;
        return {0., 0.};
    }
};

class Noise
{
public:
    float get()
    {
        return 100. * noise(gen);
    }
private:
    static inline std::mt19937 gen{std::random_device{}()};
    std::normal_distribution<float> noise{0., 1.};
};

class BrownianPath: public sf::Drawable, public sf::Transformable
{
public:
    explicit BrownianPath(sf::Vector2u const &size, std::unique_ptr<Potential> p):
    width{size.x},
    height{size.y},
    energyPotential{std::move(p)},
    points{}
    {
        points.emplace_back(static_cast<float>(width) / 2., static_cast<float>(height) / 2.);
    }
    void setSize(sf::Vector2u const & size)
    {
        width = size.x;
        height = size.y;
    }
    bool timeToPropagate()
    {
        auto now = std::chrono::system_clock::now();
        auto elapsed = getElapsed(lastUpdate, now);
        if (elapsed >= waitTime.count())
        {
            lastUpdate = now;
            return true;
        }
        return false;
    }
    void propagate()
    {
        auto const &last = points[points.size() - 1];
        auto const &potentialGradient = energyPotential->gradient(last);
        auto const sqrtPropagationTime = std::sqrt(propagationTime);
        auto const newX = last.first - potentialGradient.first * propagationTime + sqrtPropagationTime * noiseA.get();
        auto const newY = last.second - potentialGradient.second * propagationTime + sqrtPropagationTime * noiseB.get();
        if (newX < width && newY < height) points.emplace_back(newX, newY);
    }
private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        states.texture = NULL;
        if (points.size() == 1)
        {
            sf::VertexArray point{sf::Points, 1};
            point[0].position = {points[0].first, points[0].second};
            target.draw(point);
            return;
        }
        sf::VertexArray lines{sf::LinesStrip, points.size()};
        // sf::VertexArray lines{sf::Points, points.size()};
        for (std::size_t i = 0; i < points.size(); ++i)
        {
            lines[i].position = sf::Vector2f{points[i].first, points[i].second};
            lines[i].color = sf::Color::Red;
        }
        target.draw(lines);
    }
    std::size_t width, height;
    std::unique_ptr<Potential> energyPotential;
    Noise noiseA{};
    Noise noiseB{};
    TimePoint lastUpdate{};
    std::vector<std::pair<float, float>> points{};
    std::chrono::milliseconds const waitTime{100};
    float const propagationTime{1e-2};
};

int main()
{
    sf::RenderWindow win{sf::VideoMode{800, 800}, "Brownian Motion"};
    BrownianPath path{win.getSize(), std::make_unique<Flat>()};
    while (win.isOpen())
    {
        sf::Event event;
        while (win.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) win.close();
            if (event.type == sf::Event::Resized) path.setSize(win.getSize());
        }
        if (path.timeToPropagate()) path.propagate();
        win.clear(sf::Color::Black);
        win.draw(path);
        win.display();
    }
}