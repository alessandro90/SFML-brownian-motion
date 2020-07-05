#include "SFML/Graphics.hpp"
#include <vector>
#include <chrono>
#include <memory>
#include <utility>
#include <iostream>
#include "coordinate2d.hpp"
#include "potential.hpp"
#include "noise.hpp"

using TimePoint = std::chrono::system_clock::time_point;

auto getElapsed(TimePoint const &start, TimePoint const &end)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

class BrownianPath: public sf::Drawable, public sf::Transformable
{
public:
    explicit BrownianPath(sf::Vector2u const &size, std::unique_ptr<Potential> p, sf::Color color):
    width{size.x},
    height{size.y},
    energyPotential{std::move(p)},
    color{color}
    {
        points.push_back(Coordinate2D{
            static_cast<float>(width) / 2.f, static_cast<float>(height) / 2.f
        });
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
        Coordinate2D noises{noiseA.get(), noiseB.get()};
        Coordinate2D newPos = last - potentialGradient * propagationTime + sqrtPropagationTime * noises;
        if (newPos.x < width && newPos.y < height) points.push_back(newPos);
    }
private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        states.texture = NULL;
        if (points.size() == 1)
        {
            sf::VertexArray point{sf::Points, 1};
            point[0].position = sf::Vector2f{points[0].x, points[0].y};
            target.draw(point);
            return;
        }
        sf::VertexArray lines{sf::LinesStrip, points.size()};
        for (std::size_t i = 0; i < points.size(); ++i)
        {
            lines[i].position = sf::Vector2f{points[i].x, points[i].y};
            lines[i].color = color;
        }
        target.draw(lines);
    }
    std::size_t width, height;
    std::unique_ptr<Potential> energyPotential;
    sf::Color color;
    Noise noiseA{};
    Noise noiseB{};
    TimePoint lastUpdate{};
    std::vector<Coordinate2D> points{};
    std::chrono::milliseconds const waitTime{100};
    float const propagationTime{1e-2};
};

int main()
{
    sf::RenderWindow win{sf::VideoMode{800, 800}, "Brownian Motion"};
    auto size = win.getSize();
    BrownianPath path{size, std::make_unique<Flat>(), sf::Color::Red};
    BrownianPath path1{size, std::make_unique<SingleWell>(1.f, 1.f), sf::Color::Green};
    while (win.isOpen())
    {
        sf::Event event;
        while (win.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) win.close();
            if (event.type == sf::Event::Resized) path.setSize(win.getSize());
        }
        if (path.timeToPropagate()) path.propagate();
        if (path1.timeToPropagate()) path1.propagate();
        win.clear(sf::Color::Black);
        win.draw(path);
        win.draw(path1);
        win.display();
    }
}