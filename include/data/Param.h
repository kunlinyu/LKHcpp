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

#define UPDATE_FIELD(new_value, default_value, field) \
  if (new_value.field != default_value.field) field = new_value.field;

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

  // ==== candidate set parameters ====
  int ascent_candidates = 50;  // Number of candidate edges to be associated
                               // with each node during the ascent
  int max_candidates =
      5;  // Maximum number of candidate edges to be associated with each node
  bool candidate_set_symmetric = false;

  // ==== popomusic parameters ====
  int popmusic_initial_tour = 0;   // Specifies whether the first POPMUSIC tour
                                   // is used as initial tour for LK
  int popmusic_max_neighbors = 5;  // Maximum number of nearest neighbors used
                                   // as candidates in iterated 3-opt
  int popmusic_sample_size = 10;   // The sample size
  int popmusic_solutions = 50;     // Number of solutions to generate
  int popmusic_trials = 1;         // Maximum trials used for iterated 3-opt

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
  int max_swaps =
      0;  // Maximum number of swaps made during the search for a move

  // ==== other parameters ====
  unsigned seed = 1;  // Initial seed for random number generation
  int salesmen = 1;

  void Patch(const Param& p) {
    const Param d;
    UPDATE_FIELD(p, d, tsplib_filename);
    UPDATE_FIELD(p, d, tour_filename);
    UPDATE_FIELD(p, d, candidate_set_type);
    UPDATE_FIELD(p, d, initial_tour_algorithm);
    UPDATE_FIELD(p, d, tree_type);
    UPDATE_FIELD(p, d, move_type);
    UPDATE_FIELD(p, d, subsequent_move_type);
    UPDATE_FIELD(p, d, nonsequential_move_type);
    UPDATE_FIELD(p, d, gain23_used);
    UPDATE_FIELD(p, d, gain_criterion_used);
    UPDATE_FIELD(p, d, runs);
    UPDATE_FIELD(p, d, max_trials);
    UPDATE_FIELD(p, d, stop_at_optimum);
    UPDATE_FIELD(p, d, time_limit);
    UPDATE_FIELD(p, d, total_time_limit);
    UPDATE_FIELD(p, d, known_optimum);
    UPDATE_FIELD(p, d, ascent_candidates);
    UPDATE_FIELD(p, d, max_candidates);
    UPDATE_FIELD(p, d, candidate_set_symmetric);
    UPDATE_FIELD(p, d, popmusic_initial_tour);
    UPDATE_FIELD(p, d, popmusic_max_neighbors);
    UPDATE_FIELD(p, d, popmusic_sample_size);
    UPDATE_FIELD(p, d, popmusic_solutions);
    UPDATE_FIELD(p, d, popmusic_trials);
    UPDATE_FIELD(p, d, excess);
    UPDATE_FIELD(p, d, initial_period);
    UPDATE_FIELD(p, d, initial_step_size);
    UPDATE_FIELD(p, d, restricted_search);
    UPDATE_FIELD(p, d, max_breadth);
    UPDATE_FIELD(p, d, max_swaps);
    UPDATE_FIELD(p, d, seed);
    UPDATE_FIELD(p, d, salesmen);
  }
  void adjust(int dimension) {
    if (seed == 0) seed = (unsigned)(std::time(nullptr) * (size_t)(&seed));
    if (max_swaps == 0) max_swaps = dimension;
    if (max_candidates > dimension - 1)
      max_candidates = dimension - 1;
    else {
      if (ascent_candidates > dimension - 1) ascent_candidates = dimension - 1;
      if (initial_period == 0) {
        initial_period = dimension / 2;
        if (initial_period < 100) initial_period = 100;
      }
      if (excess == 0) excess = 1.0 / dimension * salesmen;
      if (max_trials == 0) max_trials = dimension;
    }
    if (popmusic_max_neighbors > dimension - 1)
      popmusic_max_neighbors = dimension - 1;
    if (popmusic_sample_size > dimension) popmusic_sample_size = dimension;

    if (salesmen < 1 or salesmen > dimension)
      throw std::invalid_argument("Too many salesmen/vehicles (>= DIMENSION)");

    if (subsequent_move_type == 0) {
      subsequent_move_type = move_type;
    }
    int K =
        move_type >= subsequent_move_type ? move_type : subsequent_move_type;
    if (nonsequential_move_type == -1 || nonsequential_move_type > K)
      nonsequential_move_type = K;
  }
  /*
   * The TourFilenameWithCost function returns a copy of the string Name where
   * all occurrences of the character '$' have been replaced by Cost.
   */
  std::string TourFilenameWithCost(GainType Cost) const {
    std::string result = tour_filename;
    const std::string cost_str = std::to_string(Cost);
    size_t pos = 0;
    while ((pos = result.find('$', pos)) != std::string::npos) {
      result.replace(pos, 1, cost_str);
      pos += cost_str.length();
    }
    return result;
  }
};

extern thread_local Param param;
