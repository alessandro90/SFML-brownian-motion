#include "SFML/Graphics.hpp"
#include <cmath>
#include <chrono>
#include <memory>
#include <utility>
#include <array>
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

class BrownianPath: public sf::Drawable, public sf::Transformable
{
public:
    explicit BrownianPath(sf::Vector2u const &size, std::unique_ptr<Potential> p, sf::Color color):
    width{size.x},
    height{size.y},
    energyPotential{std::move(p)},
    color{color}
    {
        points.push_back(sf::Vector2f{
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
        sf::Vector2f noises{noiseA.get(), noiseB.get()};
        sf::Vector2f newPos = last - potentialGradient * propagationTime + std::sqrt(propagationTime) * noises;
        if (newPos.x < width && newPos.y < height)
        {
            points.push_back(newPos);
        }
    }
private:
    struct Drawer
    {
        BrownianPath *p;
        virtual ~Drawer() = default;
        Drawer(BrownianPath *p): p{p} {}
        virtual void draw(sf::RenderTarget &target) = 0;
    };
    struct DrawerLines: Drawer
    {
        explicit DrawerLines(BrownianPath *p): Drawer{p} {}
        void draw(sf::RenderTarget &target) override
        {
            sf::VertexArray lines{sf::LinesStrip, p->points.size()};
            for (std::size_t i = 0; i < p->points.size(); ++i)
            {
                lines[i].position = p->points[i];
                lines[i].color = p->color;
            }
            target.draw(lines);
        }
    };
    struct DrawerPoint: Drawer
    {
        explicit DrawerPoint(BrownianPath *p): Drawer{p} {}
        void draw(sf::RenderTarget &target) override
        {
            if (p->points.size() > 1)
            {
                p->drawer = &p->drawerLines;
                p->drawer->draw(target);
                return;
            }
            sf::VertexArray point{sf::Points, 1};
            point[0].position = sf::Vector2f{p->points[0].x, p->points[0].y};
            target.draw(point);
        }
    };
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        states.texture = NULL;
        drawer->draw(target);
    }
    std::size_t width, height;
    std::unique_ptr<Potential> energyPotential;
    sf::Color color;
    DrawerPoint drawerPoint{this};
    DrawerLines drawerLines{this};
    Drawer *drawer{&drawerPoint};
    Noise noiseA{};
    Noise noiseB{};
    TimePoint lastUpdate{};
    CircularArray<sf::Vector2f, 100> points{};
    std::chrono::milliseconds const waitTime{100};
    float const propagationTime{1e-2};
};

int main()
{
    sf::RenderWindow win{sf::VideoMode{800, 800}, "Brownian Motion"};
    auto size = win.getSize();
    // BrownianPath path1{size, std::make_unique<SingleWell>(size), sf::Color::Green};
    BrownianPath path{size, std::make_unique<Flat>(), sf::Color::Red};
    BrownianPath path1{size, std::make_unique<Flat>(), sf::Color::Green};
    BrownianPath path2{size, std::make_unique<Flat>(), sf::Color::Blue};
    BrownianPath path3{size, std::make_unique<Flat>(), sf::Color::Magenta};
    BrownianPath path4{size, std::make_unique<Flat>(), sf::Color::Cyan};
    std::array drawables{&path, &path1, &path2, &path3, &path4};
    
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
                std::for_each(drawables.begin(), drawables.end(), [&](auto &d) {
                    d->setSize(win.getSize());
                });
                break;
            default:
                break;
            }
        }
        win.clear(sf::Color::Black);
        for (auto d : drawables)
        {
            if (d->timeToPropagate()) d->propagate();
            win.draw(*d);
        }
        win.display();
    }
}
