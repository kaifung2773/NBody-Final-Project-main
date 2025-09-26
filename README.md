# N-Body Final Project

High-performance N-body simulation written primarily in C++, intended as a final project exploring numerical methods and performance optimization for simulating many-body dynamics under Newtonian gravity.


## Overview

This project simulates the evolution of a system of particles interacting via gravitational forces. It is designed to be a foundation for experiments in:
- Numerical integration schemes (e.g., explicit/implicit Euler, Leapfrog/Verlet, Runge–Kutta, etc.)
- Precision versus performance trade-offs
- Parallelization and scaling on modern CPUs (and optionally accelerators, if implemented)
- Data layout and cache efficiency


## Features

Replace or confirm the list below to match the current codebase:
- Deterministic Newtonian N-body dynamics with configurable time step
- Pluggable integrator(s): e.g., Euler, Semi-implicit Euler, Leapfrog, RK4
- Optional gravitational softening to prevent singularities
- Initial condition generation (random or file-based)
- Periodic or open boundary conditions
- CSV/JSON/Binary output for trajectories or final state
- Command-line interface for simulation parameters
- Optional parallelization using OpenMP or similar

## Quick start

### Prerequisites

- CMake (>= 3.16 recommended)
- A C++17 (or newer) compatible compiler (GCC, Clang, or MSVC)
- Optional:
  - OpenMP-capable compiler for multicore execution
  - Python 3 with NumPy/Matplotlib for post-processing and plotting (if you plan to visualize results)

### Configure and build

```bash
# From the repository root
mkdir -p build
cd build

# Configure (add flags as needed)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . --config Release --parallel
```

### Run

The executable name may differ; adjust the command accordingly.

```bash
# Example: run a short simulation with default parameters
./nbody

# Example with parameters (replace with the actual CLI options supported)
./nbody \
  --bodies 10000 \
  --steps 1000 \
  --dt 1e-3 \
  --softening 1e-2 \
  --input init.csv \
  --output results/
```

If OpenMP or threading is enabled, you can control threads via environment variables:
```bash
export OMP_NUM_THREADS=8
./nbody [args...]
```

## Command-line interface

Update this section to match the actual flags supported by the program:

- `--bodies, -n <int>`: Number of particles
- `--steps, -s <int>`: Number of integration steps
- `--dt <float>`: Time step
- `--softening <float>`: Gravitational softening parameter
- `--theta <float>`: Opening angle (if using Barnes–Hut)
- `--input <path>`: Path to initial conditions file (CSV/JSON/Binary)
- `--output <dir>`: Directory for outputs (snapshots/logs)
- `--seed <int>`: RNG seed for reproducibility
- `--integrator <name>`: Integration method (e.g., euler, leapfrog, rk4)
- `--precision <single|double>`: Floating-point precision
- `--profile`: Enable basic performance metrics

## Input/Output formats

Describe actual formats used by the project. Example:

- Input CSV (example):
  ```
  id,mass,x,y,z,vx,vy,vz
  0,1.0,0.0,0.0,0.0,0.0,0.1,0.0
  1,1.0,1.0,0.0,0.0,0.0,-0.1,0.0
  ```
- Output:
  - `state_####.csv` snapshots
  - `metrics.json` with wall time, step time, FLOP estimates

## Performance tips

- Build with `-DCMAKE_BUILD_TYPE=Release` (or `RelWithDebInfo`)
- Enable OpenMP if supported: `-DUSE_OPENMP=ON`
- Prefer SoA (structure of arrays) for particle data to improve cache locality
- Experiment with vectorization flags (e.g., `-O3 -march=native` on GCC/Clang)
- Consider cell lists or Barnes–Hut for O(N log N) scaling with large N

## Project structure

Update to reflect the actual layout:

```
.
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── integrators/
│   ├── physics/
│   ├── io/
│   └── utils/
├── include/               # Public headers (if applicable)
├── scripts/               # Helper shell or Python scripts
├── data/                  # Sample inputs
├── tests/                 # Unit/validation tests
└── build/                 # Out-of-source build directory (generated)
```

## Testing

If tests exist, document how to run them. Example:

```bash
# Configure with tests enabled
cmake -S . -B build -DENABLE_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Reproducibility

- Record compiler version, flags, and random seeds in outputs
- Use fixed seeds (`--seed`) for deterministic runs
- Capture the exact commit hash in logs (can be embedded by CMake at build time)

## Roadmap

- [ ] Document exact integrators and parameters implemented
- [ ] Provide example input datasets and reference outputs
- [ ] Add unit tests and physical invariants checks (e.g., energy drift)
- [ ] Benchmark suite and scaling plots
- [ ] Optional: Barnes–Hut / FMM / GPU backend

## Troubleshooting

- Build errors:
  - Ensure CMake and compiler versions meet minimum requirements
  - Try clearing the build directory and re-configuring
- Runtime performance:
  - Use Release builds
  - Adjust thread count/environment variables
  - Profile to identify hotspots

## Contributing

1. Fork the repo and create a feature branch
2. Follow the existing code style and add tests where feasible
3. Open a pull request with a clear description and motivation

## License

No license file is currently present in this repository. Consider adding a license (e.g., MIT, Apache-2.0, or GPL) to clarify usage and distribution.

## Acknowledgments

- Classical N-body problem literature and community resources
- Numerical methods and HPC course materials
- Open-source tooling: CMake, compilers, and profilers

---
If you maintain this repository, please replace placeholders with the project’s actual details (flags, file names, integrators, and directory layout).
