# LKHcpp

LKHcpp is a C++ reimplementation of the well-known Traveling Salesman Problem (TSP) solver LKH3.  
The project translates the core techniques from LKH3 into modern C++, aiming to preserve algorithmic behavior while providing a cleaner, extensible codebase suitable for experimenting with TSP variants.

## Key features
- Core LK heuristic translated to C++.
- Local search moves from 2-opt up to 5-opt.
- Minimum 1-tree construction and handling.
- Subgradient optimization for candidate set and bound improvements.
- PopMUSIC population-based multi-start optimization.

## Design goals
- Faithful translation of LKH3's core algorithms with attention to performance and correctness.
- Modular and extensible architecture to facilitate adding and experimenting with other problem variants (e.g., time windows, capacity constraints, prize-collecting variants).
- Clear separation between core optimization routines and problem-specific components.

## Building and usage
- Requires a C++17-compatible compiler and CMake.
- Typical build:
  ```
  mkdir build
  cd build
  cmake ..
  cmake --build .
  ```
- After building, run the solver with an instance file (TSP / TSPLIB format) and optional configuration flags. See source for CLI options and configuration examples.

## Contributing
Contributions are welcome. Preferred workflow:
- Open an issue to discuss significant changes or new features.
- Submit small, focused pull requests with tests where applicable.
- Keep design changes modular to preserve extensibility.
