// File: NBody.h
// Declares functions for CPU-based N-Body computation

#ifndef NBODY_H
#define NBODY_H

#include <vector>
#include <SFML/Graphics.hpp>  // for sf::Color
#include "Body.h"

// Compute pairwise gravitational accelerations
void compute_forces(std::vector<Body>& bodies, const float G, const float eps);

// Integrate positions and velocities over timestep dt and wrap around edges
void integrate_bodies(std::vector<Body>& bodies, const float dt, const int width, const int height);

// Run one full CPU simulation step (forces + integration)
void runCpuComputation(std::vector<Body>& bodies, const float G, const float eps, const float dt, const int width, const int height);

#endif