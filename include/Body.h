// File: Body.h
// Defines the Body and BodiesSOA types for the N-Body simulation

#ifndef BODY_H
#define BODY_H

#include <random>
#include <vector>
#include <cstddef>

// Represents a single particle (body) in the simulation
struct Body {
    float x;
    float y;
    
    float velocity_x;
    float velocity_y;

    float acceleration_x;
    float acceleration_y;

    float mass;

    bool operator==(const Body &a) const
    {
        return (x == a.x && y == a.y);
    }
};

// Structure-of-arrays layout for more efficient GPU or vectorized processing
struct BodiesSOA
{
    std::vector<float> x;
    std::vector<float> y;

    std::vector<float> vx;
    std::vector<float> vy;

    std::vector<float> ax;
    std::vector<float> ay;

    std::vector<float> mass;

    size_t size;

    BodiesSOA(size_t n)
        : x(n), y(n), vx(n), vy(n), ax(n), ay(n), mass(n), size(n) {}
};

Body randomBody(std::mt19937 &rng, int width, int height);

    Body centralBody(float mass, int width, int height);

#endif