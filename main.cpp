// File: main.cpp
// Entry point for N-Body simulation: sets up bodies, chooses CPU/GPU path, and starts rendering

#include <iostream>     // std::cout, std::cin
#include <vector>       // std::vector
#include <random>       // std::mt19937
#include "Body.h"       // randomBody(), centralBody(), Body
#include "NBody.h"      // compute_forces(), integrate_bodies(), G, eps, dt (if you’ve exposed them here)
#include "SFML.h"       // render_bodies()
#include "GpuComputation.h"

// Constants
constexpr float G = 1.f;
constexpr float dt = 0.1f;
constexpr float eps = 1e-1f;
constexpr size_t n_bodies = 50;
constexpr float center_mass = 1000.f;

const int WIDTH = 1920;
const int HEIGHT = 1080;

int main()
{
    // initialize random generator and create a set of bodies
    std::vector<Body> bodies;
    static std::mt19937 rng(42);

    for (int i = 0; i < n_bodies; ++i) {
        bodies.push_back(randomBody(rng, WIDTH, HEIGHT));
    }

    // create a central massive body at the simulation center
    Body centerBody = centralBody(center_mass ,WIDTH, HEIGHT);
    bodies.push_back(centerBody);

    // ask user whether to use GPU (OpenCL) acceleration
    std::cout << "Do you want to render with OpenCL? (1/0)" << std::endl;
    int open_cl_render;
    std::cin >> open_cl_render;

    if (open_cl_render) {
        // initialize GPU resources and run simulation on GPU
        initGpuComputation(bodies.size());

        // render loop: use GPU-accelerated computation to update and draw bodies
        render_bodies(runGpuComputation, bodies, G, eps, dt, WIDTH, HEIGHT);
        // clean up GPU resources after rendering
        cleanupGpuComputation();
    }
    else {
        // run simulation on CPU and render
        render_bodies(runCpuComputation, bodies, G, eps, dt, WIDTH, HEIGHT);
    }

    return 0;
}