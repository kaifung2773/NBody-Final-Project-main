// File: NBody.cpp
// Implements CPU-based N-Body simulation
//  - compute_forces: calculates gravitational accelerations with softening
//  - integrate_bodies: updates velocities and positions, applies toroidal wrapping, skips central mass
//  - runCpuComputation: performs one simulation step by chaining forces and integration

#include <cstddef>    // for size_t
#include <vector>     // for std::vector
#include <cmath>      // for std::pow, std::sqrt

#include "Body.h"     // Body struct, randomBody(), centralBody()
#include "NBody.h"    // declarations of compute_forces(), integrate_bodies()

// Compute pairwise gravitational accelerations for each body
void compute_forces(std::vector<Body>& bodies, const float G, const float eps)
{
    for(Body& current_body : bodies)
    {
        // ensure old acceleration is cleared before new calculation
        current_body.acceleration_x = current_body.acceleration_y = 0;
    }

    // compute forces between every pair of bodies
    for(Body& current_body : bodies)
    {
        for(Body& other_body : bodies)
        {
            if (current_body == other_body) continue;

            // displacement from current body to other body
            float dx = other_body.x - current_body.x;
            float dy = other_body.y - current_body.y;

            // compute softened inverse distance cubed for numerical stability
            float inv_distance_cubed = pow(1/sqrt(pow(dx,2) + pow(dy,2) + pow(eps,2)), 3);

            // Newtonian gravitational force magnitude
            float force = G * other_body.mass * inv_distance_cubed;

            // accumulate acceleration contributions
            current_body.acceleration_x += dx * force;
            current_body.acceleration_y += dy * force;
        }
    }
}

// update body velocities and positions, applying wrapping and skipping central mass
void integrate_bodies(std::vector<Body>& bodies, const float dt, const int width, const int height)
{
    for(Body& current_body : bodies) 
    {
        // Skip the heavy central to keep it fixed
        if (current_body.mass >= 1000.f) continue;

        // update velocity based on acceleration
        current_body.velocity_x += current_body.acceleration_x * dt;
        current_body.velocity_y += current_body.acceleration_y * dt;

        // update position based on updated velocity
        current_body.x += current_body.velocity_x * dt;
        current_body.y += current_body.velocity_y * dt;

        // wrap around horizontally to maintain toroidal space
        if (current_body.x < -width/2) current_body.x += width;
        else if (current_body.x > width/2) current_body.x -= width;

        // wrap around vertically to maintain toroidal space
        if (current_body.y < -height/2) current_body.y += height;
        else if (current_body.y > height/2) current_body.y -= height;
    }
}

// Perform one CPU simulation step: compute forces then integrate bodies
void runCpuComputation(std::vector<Body>& bodies, const float G, const float eps, const float dt, const int width, const int height) {

    compute_forces(bodies, G, eps);
    integrate_bodies(bodies, dt, width, height);
}
