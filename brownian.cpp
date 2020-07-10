#include "SFML/Graphics.hpp"
#include <cmath>
#include <chrono>
#include <memory>
#include <utility>
#include <list>
#include <algorithm>
#include "coordinate2d.hpp"
#include "potential.hpp"
#include "noise.hpp"
#include "circular_array.hpp"

using TimePoint = std::chrono::system_clock::time_point;

auto getElapsed(TimePoint const &start, TimePoint const &end)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

class BrownianPath : public sf::Drawable, public sf::Transformable
{
public:
    explicit BrownianPath(sf::Vector2u const &size,
                          sf::Vector2f const &startPoint,
                          std::unique_ptr<Potential> p,
                          sf::Color color) : width{size.x},
                                             height{size.y},
                                             energyPotential{std::move(p)},
                                             color{color}
    {
        points.push_back(startPoint);
        propagate();
    }
    void setSize(sf::Vector2u const &size)
    {
        width = size.x;
        height = size.y;
        energyPotential->setSize(size);
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
        sf::Vector2f noises{noiseA.get(), noiseB.get()};
        sf::Vector2f newPos = last - potentialGradient * propagationTime + std::sqrt(propagationTime) * noises;
        if (insideView(newPos))
        {
            points.push_back(newPos);
        }
    }

private:
    bool insideView(sf::Vector2f const &v) const
    {
        return 0 < v.x && v.x < width && 0 < v.y && v.y < height;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        states.texture = NULL;
        sf::VertexArray lines{sf::LinesStrip, points.size()};
        for (std::size_t i = 0; i < points.size(); ++i)
        {
            lines[i].position = points[i];
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
    CircularArray<sf::Vector2f, 100> points{};
    std::chrono::milliseconds const waitTime{100};
    float const propagationTime{1e-2};
};

int main()
{
    sf::RenderWindow win{sf::VideoMode{600, 600}, "Brownian Motion"};
    std::list<BrownianPath> paths{};
    ColorPicker colorPicker{};

    while (win.isOpen())
    {
        sf::Event event;
        while (win.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                win.close();
                break;
            case sf::Event::Resized:
                // Scale the view to fit the new resolution
                // https://www.sfml-dev.org/tutorials/2.2/graphics-view.php#showing-more-when-the-window-is-resized
                win.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                std::for_each(paths.begin(), paths.end(), [&](auto &d) {
                    d.setSize(win.getSize());
                });
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    auto size = win.getSize();

                    paths.emplace_back(
                        size,
                        sf::Vector2f{static_cast<float>(event.mouseButton.x),
                                     static_cast<float>(event.mouseButton.y)},
                        std::make_unique<InverseSingleWell>(size),
                        colorPicker.pick());
                }
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Delete)
                {
                    if (!paths.empty())
                    {
                        paths.erase(paths.begin());
                    }
                }
                break;
            default:
                break;
            }
        }
        win.clear(sf::Color{50, 50, 50});
        for (auto &d : paths)
        {
            if (d.timeToPropagate())
                d.propagate();
            win.draw(d);
        }
        win.display();
    }
}
