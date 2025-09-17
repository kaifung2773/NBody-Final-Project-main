// File: SFML.h
// Declares rendering routines and helpers for displaying bodies with SFML

#ifndef SFML_H
#define SFML_H

#include <functional>
#include <vector>
#include "Body.h"
#include <SFML/Graphics.hpp>  // for sf::Color, sf::Time, etc.

// Draws all bodies in a window, calling 'computations' each frame to update positions
void render_bodies(
    std::function<void(std::vector<Body>&, float, float, float, int, int)> computations,
    std::vector<Body>& bodies,
    const float G,
    const float eps,
    const float dt,
    const int width,
    const int height);

// Maps a body's mass to an SFML color for visualization
sf::Color mass_to_color(float mass);

// Computes the circular orbital speed for mass M at distance r
float orbital_velocity_scalar(float M, float r);

#endif // SFML_H