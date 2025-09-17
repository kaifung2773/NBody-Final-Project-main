// File: SFML.cpp
// Renders N-body simulation using SFML and displays FPS

#include <SFML/Graphics.hpp>  // SFML main header
#include "SFML.h"
#include <iostream>
#include "NBody.h"

constexpr float TARGET_FPS = 165.f;  // target frames per second
const sf::Time FRAME_DURATION = sf::seconds(1.f / TARGET_FPS);

// Main rendering function: takes a computation callback to update bodies each frame
void render_bodies(
    std::function<void(std::vector<Body>&, const float, const float, const float, const int, const int)> compute,
    std::vector<Body>& bodies,
    const float G,
    const float eps,
    const float dt,
    const int width,
    const int height) {
    // create window
    sf::RenderWindow window(sf::VideoMode(width, height), "N-Body Simulation");

    // load font for FPS display
    sf::Font font;
    if (!font.loadFromFile("../OpenSans-Bold.ttf")) {
        std::cerr << "Failed to load font\n";
    }
    sf::Text fpsText("", font, 18);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 5);

    sf::Clock frameClock;  // for frame limiting
    sf::Clock fpsClock;    // for FPS calculation
    float lastFPS = 0.0f;

    // main loop
    while (window.isOpen()) {
        // handle events
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) 
                window.close();
        }

        // update simulation state
        compute(bodies, G, eps, dt, width, height);

        // clear screen
        window.clear(sf::Color::Black);

        // draw each body as a circle
        for (const Body& b : bodies) {
            sf::CircleShape circle(b.mass > 50.0f ? 6 : 2);
            circle.setFillColor(mass_to_color(b.mass));
            circle.setPosition(width / 2 + b.x, height / 2 + b.y);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            window.draw(circle);
        }

        // calculate and display FPS
        float elapsed = fpsClock.restart().asSeconds();
        lastFPS = 1.0f / elapsed;
        fpsText.setString("FPS: " + std::to_string((int)lastFPS));
        window.draw(fpsText);

        // show on screen
        window.display();

        // limit to target frame rate
        sf::Time elapsed2 = frameClock.getElapsedTime();
        if (elapsed2 < FRAME_DURATION)
            sf::sleep(FRAME_DURATION - elapsed2);
        frameClock.restart();
    }
}

// Convert mass to a color gradient from blue to pink
sf::Color mass_to_color(float mass) {
    float norm = std::min(1.0f, mass / 10.0f);
    return sf::Color(255 * norm, 50, 255 * (1 - norm));
}

// Compute circular orbital speed for mass M at distance r (assumes G=1)
float orbital_velocity_scalar(float M, float r) {
    return std::sqrt(1.0f * M / r);
}