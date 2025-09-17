// File: GpuComputation.cpp
// Handles GPU acceleration for N-body simulation using OpenCL

#include "GpuComputation.h"
#include <iostream>
#include <fstream>
#include <sstream>
#define CL_TARGET_OPENCL_VERSION 120
#include <gegl-0.4/opencl/cl.h>


// GPU runtime state: holds OpenCL context, queue, program, kernels, buffers, and number of bodies
static cl_context        s_context        = nullptr;
static cl_command_queue  s_queue          = nullptr;
static cl_program        s_program        = nullptr;
static cl_kernel         s_k_forces       = nullptr;
static cl_kernel         s_k_integrate    = nullptr;
static cl_mem            s_buf_x          = nullptr;
static cl_mem            s_buf_y          = nullptr;
static cl_mem            s_buf_vx         = nullptr;
static cl_mem            s_buf_vy         = nullptr;
static cl_mem            s_buf_ax         = nullptr;
static cl_mem            s_buf_ay         = nullptr;
static cl_mem            s_buf_mass       = nullptr;
static size_t            s_n              = 0;

// Initialize GPU: create context, compile kernels, and allocate device buffers
void initGpuComputation(size_t n_bodies) {
    cl_int err;
    s_n = n_bodies;

    // load kernel source
    std::ifstream cl_file("../opencl/NBody.cl");
    if (!cl_file.is_open()) {
        std::cerr << "Failed to open OpenCL kernel file\n";
        return;
    }
    std::string src{ std::istreambuf_iterator<char>(cl_file),
                     std::istreambuf_iterator<char>() };
    const char* source_str = src.c_str();
    size_t source_size = src.size();

    // platform & device
    cl_platform_id platform;
    cl_uint num_platforms;
    err = clGetPlatformIDs(1, &platform, &num_platforms);
    cl_device_id device;
    cl_uint num_devices;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);

    // context & queue
    s_context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    s_queue   = clCreateCommandQueue(s_context, device, 0, &err);

    // program build
    s_program = clCreateProgramWithSource(s_context, 1, &source_str, &source_size, &err);
    err = clBuildProgram(s_program, 1, &device, NULL, NULL, NULL);

    // kernels
    s_k_forces    = clCreateKernel(s_program, "compute_forces",    &err);
    s_k_integrate = clCreateKernel(s_program, "integrate_bodies", &err);

    // buffers (no host copy here)
    size_t bytes = sizeof(float) * s_n;
    s_buf_x    = clCreateBuffer(s_context, CL_MEM_READ_WRITE, bytes, NULL, &err);
    s_buf_y    = clCreateBuffer(s_context, CL_MEM_READ_WRITE, bytes, NULL, &err);
    s_buf_vx   = clCreateBuffer(s_context, CL_MEM_READ_WRITE, bytes, NULL, &err);
    s_buf_vy   = clCreateBuffer(s_context, CL_MEM_READ_WRITE, bytes, NULL, &err);
    s_buf_ax   = clCreateBuffer(s_context, CL_MEM_READ_WRITE, bytes, NULL, &err);
    s_buf_ay   = clCreateBuffer(s_context, CL_MEM_READ_WRITE, bytes, NULL, &err);
    s_buf_mass = clCreateBuffer(s_context, CL_MEM_READ_ONLY,  bytes, NULL, &err);
}

// Execute one simulation step on GPU: upload data, run kernels, download results
void runGpuComputation(std::vector<Body> &bodies,
                       const float G,
                       const float eps,
                       const float dt,
                       const int width,
                       const int height)
{
    // pack data into structure-of-arrays for GPU
    BodiesSOA soa(s_n);
    for (size_t i = 0; i < s_n; ++i) {
        soa.x[i]    = bodies[i].x;
        soa.y[i]    = bodies[i].y;
        soa.vx[i]   = bodies[i].velocity_x;
        soa.vy[i]   = bodies[i].velocity_y;
        soa.ax[i]   = 0.f;
        soa.ay[i]   = 0.f;
        soa.mass[i] = bodies[i].mass;
    }

    size_t bytes = sizeof(float) * s_n;
    // copy input arrays to GPU buffers
    clEnqueueWriteBuffer(s_queue, s_buf_x,    CL_FALSE, 0, bytes, soa.x.data(),    0, NULL, NULL);
    clEnqueueWriteBuffer(s_queue, s_buf_y,    CL_FALSE, 0, bytes, soa.y.data(),    0, NULL, NULL);
    clEnqueueWriteBuffer(s_queue, s_buf_vx,   CL_FALSE, 0, bytes, soa.vx.data(),   0, NULL, NULL);
    clEnqueueWriteBuffer(s_queue, s_buf_vy,   CL_FALSE, 0, bytes, soa.vy.data(),   0, NULL, NULL);
    clEnqueueWriteBuffer(s_queue, s_buf_mass, CL_FALSE, 0, bytes, soa.mass.data(), 0, NULL, NULL);
    clFinish(s_queue);

    int ni = static_cast<int>(s_n);
    // set kernel arguments and run force computation
    clSetKernelArg(s_k_forces,    0, sizeof(cl_mem), &s_buf_x);
    clSetKernelArg(s_k_forces,    1, sizeof(cl_mem), &s_buf_y);
    clSetKernelArg(s_k_forces,    2, sizeof(cl_mem), &s_buf_ax);
    clSetKernelArg(s_k_forces,    3, sizeof(cl_mem), &s_buf_ay);
    clSetKernelArg(s_k_forces,    4, sizeof(cl_mem), &s_buf_mass);
    clSetKernelArg(s_k_forces,    5, sizeof(int),    &ni);
    clSetKernelArg(s_k_forces,    6, sizeof(float),  &G);
    clSetKernelArg(s_k_forces,    7, sizeof(float),  &eps);
    clEnqueueNDRangeKernel(s_queue, s_k_forces, 1, NULL, &s_n, NULL, 0, NULL, NULL);
    clFinish(s_queue);

    // set kernel arguments and run integration step
    clSetKernelArg(s_k_integrate, 0, sizeof(cl_mem), &s_buf_x);
    clSetKernelArg(s_k_integrate, 1, sizeof(cl_mem), &s_buf_y);
    clSetKernelArg(s_k_integrate, 2, sizeof(cl_mem), &s_buf_vx);
    clSetKernelArg(s_k_integrate, 3, sizeof(cl_mem), &s_buf_vy);
    clSetKernelArg(s_k_integrate, 4, sizeof(cl_mem), &s_buf_ax);
    clSetKernelArg(s_k_integrate, 5, sizeof(cl_mem), &s_buf_ay);
    clSetKernelArg(s_k_integrate, 6, sizeof(cl_mem), &s_buf_mass); 
    clSetKernelArg(s_k_integrate, 7, sizeof(int),    &ni);
    clSetKernelArg(s_k_integrate, 8, sizeof(float),  &dt);
    clSetKernelArg(s_k_integrate, 9, sizeof(int),    &width);
    clSetKernelArg(s_k_integrate,10, sizeof(int),    &height);
    clEnqueueNDRangeKernel(s_queue, s_k_integrate, 1, NULL, &s_n, NULL, 0, NULL, NULL);
    clFinish(s_queue);

    // read updated positions and velocities back to host
    clEnqueueReadBuffer (s_queue, s_buf_x,  CL_TRUE, 0, bytes, soa.x.data(),  0, NULL, NULL);
    clEnqueueReadBuffer (s_queue, s_buf_y,  CL_TRUE, 0, bytes, soa.y.data(),  0, NULL, NULL);
    clEnqueueReadBuffer (s_queue, s_buf_vx, CL_TRUE, 0, bytes, soa.vx.data(), 0, NULL, NULL);
    clEnqueueReadBuffer (s_queue, s_buf_vy, CL_TRUE, 0, bytes, soa.vy.data(), 0, NULL, NULL);

    // unpack results back into host bodies vector
    for (size_t i = 0; i < s_n; ++i) {
        bodies[i].x          = soa.x[i];
        bodies[i].y          = soa.y[i];
        bodies[i].velocity_x = soa.vx[i];
        bodies[i].velocity_y = soa.vy[i];
        bodies[i].acceleration_x = soa.ax[i];
        bodies[i].acceleration_y = soa.ay[i];
    }
}

// Clean up GPU resources: release kernels, program, queue, and memory
void cleanupGpuComputation() {
    if (s_buf_mass)  clReleaseMemObject(s_buf_mass);
    if (s_buf_ay)    clReleaseMemObject(s_buf_ay);
    if (s_buf_ax)    clReleaseMemObject(s_buf_ax);
    if (s_buf_vy)    clReleaseMemObject(s_buf_vy);
    if (s_buf_vx)    clReleaseMemObject(s_buf_vx);
    if (s_buf_y)     clReleaseMemObject(s_buf_y);
    if (s_buf_x)     clReleaseMemObject(s_buf_x);
    if (s_k_integrate) clReleaseKernel(s_k_integrate);
    if (s_k_forces)    clReleaseKernel(s_k_forces);
    if (s_program)     clReleaseProgram(s_program);
    if (s_queue)       clReleaseCommandQueue(s_queue);
    if (s_context)     clReleaseContext(s_context);
}