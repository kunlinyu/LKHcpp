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
  ```bash
  mkdir build
  cd build
  cmake ..
  # or "cmake .. -DBUILD_SHARED_LIBS=ON" for shared library
  # or "cmake .. -DCMAKE_BUILD_TYPE=Release" for release build
  # or "cmake .. -DCMAKE_TOOLCHAIN_FILE=../arm64-toolchain.cmake" for cross-compilation
  make -j16
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

## Param struct fields
This project exposes a `Param` struct (see `include/data/Param.h`) that groups runtime configuration used by the solver. Below are the main fields and their meanings - these can be set via the parameter file or command-line options (where available).

- `tsplib_filename` - Path to the input TSPLIB problem file (instance).
- `tour_filename` - Path to the (optional) initial or output tour file.

Algorithm-related:
- `candidate_set_type` - Candidate set construction method (ALPHA or POPMUSIC).
- `initial_tour_algorithm` - Algorithm used to create an initial tour (BORUVKA, GREEDY, WALK).
- `tree_type` - Tour representation: `3` = three-level tree, `2` = two-level tree, otherwise linked list.
- `move_type` - Sequential move type for local search (K >= 2 tries k-opt up to K).
- `subsequent_move_type` - Move type used for moves following the first move in a sequence.
- `nonsequential_move_type` - Nonsequential move type (L >= 4 tries l-opt up to L).
- `gain23_used` - Whether Gain23 optimization is used.
- `gain_criterion_used` - Whether the L&K gain criterion is used.

Flow control and limits:
- `runs` - Total number of runs.
- `max_trials` - Maximum number of trials in each run.
- `stop_at_optimum` - Stop a run when the tour length equals the known optimum.
- `time_limit` - Time limit in seconds for each run.
- `total_time_limit` - Total time limit in seconds (across runs).
- `known_optimum` - Known optimum tour length (if available).
- `threads` - Number of parallel jobs/threads to run.

Candidate set tuning:
- `ascent_candidates` - Number of candidate edges associated with each node during ascent.
- `max_candidates` - Maximum number of candidate edges per node.
- `candidate_set_symmetric` - Whether candidate sets are symmetric.

PopMUSIC options:
- `popmusic_initial_tour` - Use the first POPMUSIC tour as initial tour for LK.
- `popmusic_max_neighbors` - Max nearest neighbors used in iterated 3-opt.
- `popmusic_sample_size` - Sample size for PopMUSIC.
- `popmusic_solutions` - Number of solutions to generate.
- `popmusic_trials` - Maximum trials for iterated 3-opt.

Ascent tuning:
- `excess` - Maximum alpha-value for candidate edges (Excess x lower bound magnitude).
- `initial_period` - Length of the first period in ascent.
- `initial_step_size` - Initial ascent step size.

Local search pruning:
- `restricted_search` - Restrict the choice of the first edge to break.
- `max_breadth` - Max number of candidate edges considered per search level.
- `max_swaps` - Max number of swaps during move search.

Other:
- `seed` - Initial seed for random number generation.
- `salesmen` - Number of salesmen (for multi-salesman/TSP variants) - NOTE: currently not supported; setting this has no effect.

(See `include/data/Param.h` for default values and more context.)

## Code example
C++ interface
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
  std::ofstream ftour("path/to/tour.txt, std::ios::binary);
  if (ftour.is_open()) {
    result.write(ftour);
    ftour.close();
  }

  return 0;
}
```
pure C interface
```c
#include "LKH.h"

void func(const char* param_json, const char* tsplib_text) {
  LKH_Enable_LOG();
  LKH_Handle hdl = LKH_Create();

  size_t tour_len = 0;
  const char* tour_data = LKH_Solve(hdl, param_json, tsplib_text, &tour_len);
  fwrite(tour_data, 1, tour_len, stdout);

  LKH_FreeResult(tour_data);
  LKH_Destroy(hdl);
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
