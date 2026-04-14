// @file Param.h
// @brief
// @author Kunlin Yu
// @date 2026-04-06
#pragma once
#include <plog/Log.h>

#include <cfloat>
#include <climits>
#include <ctime>
#include <limits>
#include <string>

#include "type.h"

enum InitialTourAlgorithms { BORUVKA, GREEDY, WALK };
enum CandidateSetTypes { ALPHA, POPMUSIC };

struct Param {
  // ==== file parameters ====

  // Path to the input TSPLIB problem file (instance)
  std::string tsplib_filename;
  // Path to the (optional) initial or output tour file
  std::string tour_filename;

  // ==== algorithm parameters ====

  // Candidate set construction method
  CandidateSetTypes candidate_set_type = ALPHA;
  // Algorithm used to create an initial tour
  InitialTourAlgorithms initial_tour_algorithm = WALK;
  // If tree_type set to 3 the three-level tree representation is used for a
  // tour; if set to 2 the two-level tree representation is used; otherwise the
  // linked list representation is used.
  short tree_type = 2;
  // Specifies the sequential move type to be used in local search; a value K >=
  // 2 signifies that k-opt moves are tried for k <= K.
  int move_type = 5;
  // Specifies the move type to be used for all moves following the first move
  // in a sequence of moves; K >= 2 signifies that a K-opt move is to be used.
  int subsequent_move_type = 0;
  // Specifies the nonsequential move type to be used in local search; a value L
  // >= 4 signifies that nonsequential l-opt moves are tried for l <= L.
  int nonsequential_move_type = 1;
  // Specifies whether Gain23 is used
  bool gain23_used = true;
  // Specifies whether L&K's gain criterion is used
  bool gain_criterion_used = true;

  // ==== flow control parameters ====

  // Total number of runs
  size_t runs = 10;
  // Maximum number of trials in each run
  size_t max_trials = 0;
  // Specifies whether a run will be terminated if the tour length becomes equal
  // to Optimum
  bool stop_at_optimum = true;
  // Time limit in seconds for each run
  double time_limit = DBL_MAX;
  // Total time limit in seconds
  double total_time_limit = DBL_MAX;
  // Known optimum tour length
  GainType known_optimum = std::numeric_limits<GainType>::min();
  // Number of parallel jobs to run
  int threads = 1;

  // ==== candidate set parameters ====

  // Number of candidate edges to be associated with each node during the ascent
  size_t ascent_candidates = 50;
  // Maximum number of candidate edges to be associated with each node
  size_t max_candidates = 5;
  // Whether the candidate set is symmetric
  bool candidate_set_symmetric = false;

  // ==== popomusic parameters ====

  // Specifies whether the first POPMUSIC tour is used as initial tour for LK
  bool popmusic_initial_tour = false;
  // Maximum number of nearest neighbors used as candidates in iterated 3-opt
  size_t popmusic_max_neighbors = 5;
  // The sample size
  size_t popmusic_sample_size = 10;
  // Number of solutions to generate
  size_t popmusic_solutions = 50;
  // Maximum trials used for iterated 3-opt
  size_t popmusic_trials = 1;

  // ==== ascent tuning parameters ====

  // Maximum alpha-value allowed for any candidate edge is set to Excess times
  // the absolute value of the lower bound of a solution tour
  double excess = 0;
  // Length of the first period in the ascent
  size_t initial_period = 0;
  // Initial step size used in the ascent
  int initial_step_size = 1;

  // ==== local search pruning limits ====

  // Specifies whether the choice of the first edge to be broken is restricted
  bool restricted_search = true;

  // The maximum number of candidate edges considered at each level of the
  // search for a move
  int max_breadth = INT_MAX;
  // Maximum number of swaps made during the search for a move
  size_t max_swaps = 0;

  // ==== other parameters ====

  // Initial seed for random number generation
  unsigned seed = 1;
  // Number of salesmen (for multi-salesman/TSP variants)
  size_t salesmen = 1;

  void Patch(const Param& p);

  void Adjust(size_t dimension);

  std::string TourFilenameWithCost(GainType Cost) const;
};

extern thread_local Param param;
