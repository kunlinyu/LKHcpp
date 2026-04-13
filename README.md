# LKHcpp

LKHcpp is a C++-style library translated from LKH3, providing the core functionality of LKH3 for solving the Traveling Salesman Problem (TSP).
The project translates the core techniques from LKH3 into modern C++, aiming to preserve algorithmic behavior while providing a cleaner, extensible codebase suitable for experimenting with TSP variants.

LKH3: http://webhotel4.ruc.dk/~keld/research/LKH-3/

## Key features
- Core LK heuristic translated to C++.
- Local search moves from 2-opt up to 5-opt.
- Minimum 1-tree construction and handling.
- Subgradient optimization for candidate set and bound improvements.
- PopMUSIC candidate set generation.
- Multi-threaded parallel solving.
- Reentrant solving methods.
- Can be built as an independent library.

## Building
- Requires a C++11-compatible compiler and CMake.
- Typical build:
  ```
  mkdir build
  cd build
  cmake ..
  cmake --build .
  ```
- After building, run the solver with the command-line options.

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

## Feature comparison: LKH3 vs LKHcpp

| Feature                             | LKH3     | LKHcpp            |
|-------------------------------------|----------|-------------------|
| Lin-Kernighan core heuristic        | &check;  | &check;           |
| 2-opt to 5-opt local search moves   | &check;  | &check;           |
| Minimum 1-tree construction         | &check;  | &check;           |
| Subgradient optimization            | &check;  | &check;           |
| PopMUSIC candidate set generation   | &check;  | &check;           |
| C-style library interface           | &check;  | &check;           |
| Multi-threaded parallel solving     | &#10008; | &check;           |
| Reentrant solving API               | &#10008; | &check;           |
| Build as independent static library | &#10008; | &check;           |
| run time tree type selection        | &#10008; | &check;           |
| TSP Variant encoder and decoder     | &#10008; | &check;           |
| constrained problem variants        | 49       | 1(STTSP)          |
| Subproblem decomposition            | &check;  | &#10008;          |
| Genetic algorithm                   | &check;  | &#10008;          |
| k-swap kick move                    | &check;  | &#10008;          |
| K-Opt Move                          | &check;  | &#10008;          |
| Special-Opt Move                    | &check;  | &#10008;          |
| back tracking in local search       | &check;  | &#10008;          |
| backbone candidate set              | &check;  | &#10008;          |
| candidate set algorithm             | 5        | 2(alpha,popmusic) |
| tour recombination algorithm        | 3        | 1(IPT)            |
| initial tour construction algorithm | 14       | 1(WALK)           |


## Dependencies

- **CLI11**: used for parsing command-line arguments.
- **nlohmann/json**: used for reading parameter/configuration content.
- **plog**: used for runtime logging output.

These libraries are vendored directly into this project (source code included), so no extra installation is required.
