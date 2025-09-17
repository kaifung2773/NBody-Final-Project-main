// File: Body.cpp
// Implements helper functions to create simulation bodies

#include "Body.h"
#include <random>

// Generates a random body positioned randomly within the window
Body randomBody(std::mt19937& rng, int width, int height) {
    // distributions for random properties
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);  // random angle
    std::uniform_real_distribution<float> radius_dist(50.0f, std::min(width, height) / 2.f - 20.f);  // distance from center
    std::uniform_real_distribution<float> mass_dist(0.5f, 10.f);  // random mass
    std::uniform_real_distribution<float> velocity_dist(-1.0f, 1.0f);  // initial velocity components

    Body body;

    // pick random angle and radius for position
    float angle = angle_dist(rng);
    float radius = radius_dist(rng);

    // convert polar coordinates to cartesian
    body.x = radius * std::cos(angle);
    body.y = radius * std::sin(angle);

    // assign random initial velocity
    body.velocity_x = velocity_dist(rng);
    body.velocity_y = velocity_dist(rng);

    // start with no acceleration
    body.acceleration_x = 0.0f;
    body.acceleration_y = 0.0f;

    // assign mass
    body.mass = mass_dist(rng);

    return body;
}

// Creates a central, stationary body with given mass at the origin
Body centralBody(float mass, int width, int height) {
    // central body at center with no movement
    Body body;
    body.x = 0.f;
    body.y = 0.f;
    body.mass = mass;
    body.velocity_x = 0.f;
    body.velocity_y = 0.f;
    body.acceleration_x = 0.f;
    body.acceleration_y = 0.f;

    return body;
}