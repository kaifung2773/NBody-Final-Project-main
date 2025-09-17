/* File: NBody.cl
 * OpenCL kernels for N-Body simulation:
 * - compute_forces: calculates gravitational accelerations for each body
 * - integrate_bodies: updates velocities and positions, skipping central mass, with wrap-around
 */

// Kernel to compute pairwise gravitational accelerations
__kernel void compute_forces(
    __global float* x,
    __global float* y,
    __global float* ax,
    __global float* ay,
    __global float* mass,
    int n,
    float G,
    float eps
) {
    int i = get_global_id(0);
    if (i >= n) return;

    float axi = 0.0f;
    float ayi = 0.0f;

    // loop over all other bodies to accumulate force
    for (int j = 0; j < n; j++) {
        if (j == i) continue; // skip self

        // compute relative vector components
        float dx = x[j] - x[i];
        float dy = y[j] - y[i];
        // compute softened inverse distance cubed
        float inv_distance_cubed = pow(1/sqrt(pow(dx,2) + pow(dy,2) + pow(eps,2)), 3);

        // gravitational force magnitude from body j
        float f = G * mass[j] * inv_distance_cubed;
        
        // accumulate acceleration contributions
        axi += dx * f;
        ayi += dy * f;
    }

    ax[i] = axi;
    ay[i] = ayi;
}

// Kernel to integrate body motion and apply toroidal wrapping
__kernel void integrate_bodies(
    __global float* x,
    __global float* y,
    __global float* vx,
    __global float* vy,
    __global float* ax,
    __global float* ay,
    __global float* mass,
    int n,
    float dt,
    int width,
    int height
) {
    int i = get_global_id(0);
    if (i >= n) return;

    // do not move the central massive body
    if (mass[i] >= 1000.0f) return;

    // update velocity by acceleration
    vx[i] += ax[i] * dt;
    vy[i] += ay[i] * dt;

    // update position by velocity
    x[i] += vx[i] * dt;
    y[i] += vy[i] * dt;

    // wrap around edges to create a toroidal (donut) world
    if (x[i] < -width/2) x[i] += width;
    else if (x[i] > width/2) x[i] -= width;

    if (y[i] < -height/2) y[i] += height;
    else if (y[i] > height/2) y[i] -= height;
}
