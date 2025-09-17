// File: GpuComputation.h
// Provides initialization, per-frame execution, and cleanup for GPU-based N-body computation

#ifndef GPU_COMPUTATION_H
#define GPU_COMPUTATION_H

#include "Body.h"
#include <vector>

// Prepare GPU resources and compile kernels for n_bodies elements
void initGpuComputation(size_t n_bodies);

// Execute one simulation step on the GPU, updating the bodies vector
void runGpuComputation(std::vector<Body>& bodies, 
                       const float G, 
                       const float eps, 
                       const float dt, 
                       const int width, 
                       const int height);

// Release all GPU resources allocated by initGpuComputation
void cleanupGpuComputation();

#endif