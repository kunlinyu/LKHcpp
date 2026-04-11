#include "data/Param.h"

thread_local Param param;

#define UPDATE_FIELD(new_value, default_value, field) \
if (new_value.field != default_value.field) field = new_value.field;

void Param::Patch(const Param& p) {
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
  UPDATE_FIELD(p, d, threads);
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

void Param::Adjust(int dimension) {
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
  int K = move_type >= subsequent_move_type ? move_type : subsequent_move_type;
  if (nonsequential_move_type == -1 || nonsequential_move_type > K)
    nonsequential_move_type = K;
}

std::string Param::TourFilenameWithCost(GainType Cost) const {
  std::string result = tour_filename;
  const std::string cost_str = std::to_string(Cost);
  size_t pos = 0;
  while ((pos = result.find('$', pos)) != std::string::npos) {
    result.replace(pos, 1, cost_str);
    pos += cost_str.length();
  }
  return result;
}