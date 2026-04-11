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
  std::string tsplib_filename;
  std::string tour_filename;

  // ==== algorithm parameters ====
  CandidateSetTypes candidate_set_type = ALPHA;
  InitialTourAlgorithms initial_tour_algorithm = WALK;
  short tree_type =
      2;  // If tree_type set to 3 the three-level tree representation is used
          // for a tour. If tree_type set to 2 the two-level tree representation
          // is used. Otherwise, the linked list representation is used.
  int move_type = 5;  // Specifies the sequential move type to be used in local
                      // search. A value K >= 2 signifies that a k-opt moves are
                      // tried for k <= K
  int subsequent_move_type = 0;  // Specifies the move type to be used for all
                                 // moves following the first move in a sequence
                                 // of moves. The value K >= 2 signifies that a
                                 // K-opt move is to be used
  int nonsequential_move_type =
      1;  // Specifies the nonsequential move type to be used in local search. A
          // value L >= 4 signifies that nonsequential l-opt moves are tried for
          // l <= L
  bool gain23_used = true;  // Specifies whether Gain23 is used
  bool gain_criterion_used =
      true;  // Specifies whether L&K's gain criterion is used

  // ==== flow control parameters ====
  int runs = 10;                // Total number of runs
  int max_trials = 0;           // Maximum number of trials in each run
  bool stop_at_optimum = true;  // Specifies whether a run will be terminated if
                                // the tour length becomes equal to Optimum
  double time_limit = DBL_MAX;  // Time limit in seconds for each run
  double total_time_limit = DBL_MAX;  // Total time limit in seconds
  GainType known_optimum =
      std::numeric_limits<GainType>::min();  // Known optimum tour length
  int threads = 1;                           // Number of parallel jobs to run

  // ==== candidate set parameters ====
  size_t ascent_candidates = 50;  // Number of candidate edges to be associated
                                  // with each node during the ascent
  size_t max_candidates =
      5;  // Maximum number of candidate edges to be associated with each node
  bool candidate_set_symmetric = false;

  // ==== popomusic parameters ====
  bool popmusic_initial_tour = false;  // Specifies whether the first POPMUSIC
                                       // tour is used as initial tour for LK
  size_t popmusic_max_neighbors = 5;  // Maximum number of nearest neighbors
                                      // used as candidates in iterated 3-opt
  size_t popmusic_sample_size = 10;  // The sample size
  size_t popmusic_solutions = 50;    // Number of solutions to generate
  size_t popmusic_trials = 1;        // Maximum trials used for iterated 3-opt

  // ==== ascent tuning parameters ====
  double excess = 0;  // Maximum alpha-value allowed for any candidate edge is
                      // set to Excess times the absolute value of the lower
                      // bound of a solution tour
  int initial_period = 0;     // Length of the first period in the ascent
  int initial_step_size = 1;  // Initial step size used in the ascent

  // ==== local search pruning limits ====
  bool restricted_search = true;  // Specifies whether the choice of the first
                                  // edge to be broken is restricted

  int max_breadth =
      INT_MAX;  // The maximum number of candidate edges considered
  // at each level of the search for a move
  size_t max_swaps =
      0;  // Maximum number of swaps made during the search for a move

  // ==== other parameters ====
  unsigned seed = 1;  // Initial seed for random number generation
  size_t salesmen = 1;

  void Patch(const Param& p);

  void Adjust(size_t dimension);

  std::string TourFilenameWithCost(GainType Cost) const;
};

extern thread_local Param param;
