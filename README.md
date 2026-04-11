# LKHcpp

LKHcpp is a C++-style library translated from LKH3, providing the core functionality of LKH3 for solving the Traveling Salesman Problem (TSP).
The project translates the core techniques from LKH3 into modern C++, aiming to preserve algorithmic behavior while providing a cleaner, extensible codebase suitable for experimenting with TSP variants.

LKH3: http://webhotel4.ruc.dk/~keld/research/LKH-3/

## Key features
- Core LK heuristic translated to C++.
- Local search moves from 2-opt up to 5-opt.
- Minimum 1-tree construction and handling.
- Subgradient optimization for candidate set and bound improvements.
- PopMUSIC for accelerated local search and candidate set generation.
- Multi-threaded parallel solving.
- Reentrant solving methods.
- Can be built as an independent library.

## Building and usage
- Requires a C++11-compatible compiler and CMake.
- Typical build:
  ```
  mkdir build
  cd build
  cmake ..
  cmake --build .
  ```
- After building, run the solver with an instance file (TSP / TSPLIB format) and optional configuration flags. See source for CLI options and configuration examples.

## Command-line usage
```text
LKHcpp [OPTIONS]
```
### Options

- `-h, --help` Print this help message and exit.

- `--param, --parameter-file TEXT:FILE` (**required**) Path to the parameter file.

- `--problem, --problem-file TEXT:FILE` Path to the problem file.

- `--tour, --tour-file TEXT` Path to the output tour file.

- `-t, --max-trials INT:NONNEGATIVE` Maximum number of trials in each run.

- `-r, --runs INT:NONNEGATIVE` Total number of runs.

- `-o, --optimum INT:NONNEGATIVE` Known optimum tour length.

- `--tree, --tree_type INT:{1,2,3}` Level of tree representation.

- `--move, --move_type INT:{2,3,4,5}` Sequential move type to be used in local search.

- `-j, --threads INT:POSITIVE` Number of threads to run in parallel.

- `-l, --trace-level ENUM:NONNEGATIVE [4]` Specifies the level `[0-6]` of detail of the output given during the solution process.  
  `0` means minimum output; higher values print more details.

- `-v, --verbose` Print verbose log.

- `-c, --color` Print colorfully log.

## Code example
```cpp
#include "LKHcpp.h"
#include "data/ParamReader.h"
#include "data/TSPLIBReader.h"

int main() {
  // read parameters
  std::ifstream fparam("path/to/param.json");
  Param param = ParamReader::ReadStream(fparam);

  // read TSPLIB instance
  std::ifstream fproblem("path/to/problem.tsp");
  TSPLIB tsplib = TSPLIBReader::Read(fproblem);

  // solve
  LKHcpp lkh;
  TourFile result = lkh.Solve(param, tsplib);

  // write result
  std::string out = param.TourFilenameWithCost(result.tour.cost);
  std::ofstream ofs(out, std::ios::binary);
  if (ofs.is_open()) {
    result.write(ofs);
    ofs.close();
  }

  return 0;
}
```

## Feature comparison: LKHcpp vs LKH3

| Feature | LKHcpp | LKH3 |
|---|--------|------|
| Lin-Kernighan core heuristic | &check;    | &check;  |
| 2-opt to 5-opt local search moves | &check;    | &check;  |
| Minimum 1-tree construction | &check;    | &check;  |
| Subgradient optimization | &check;    | &check;  |
| PopMUSIC-based optimization | &check;    | &check;  |
| Multi-threaded parallel solving | &check;    | &#10008;   |
| Reentrant solving API | &check;    | &#10008;   |
| Build as independent static library | &check;    | &#10008;   |
| C-style library interface | &#10008;     | &check;  |
| Subproblem decomposition | &#10008;     | &check;  |
| Delaunay candidate algorithm | &#10008;     | &check;  |
| Genetic algorithm | &#10008;     | &check;  |
| 40+ constrained problem variants | &#10008;     | &check;  |

## Dependencies

- **CLI11**: used for parsing command-line arguments.
- **nlohmann/json**: used for reading parameter/configuration content.
- **plog**: used for runtime logging output.

These libraries are vendored directly into this project (source code included), so no extra installation is required.
