#include <plog/Log.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "data/Param.h"

NLOHMANN_JSON_SERIALIZE_ENUM(InitialTourAlgorithms, {
                                                        {BORUVKA, "BORUVKA"},
                                                        {GREEDY, "GREEDY"},
                                                        {WALK, "WALK"},
                                                    });

NLOHMANN_JSON_SERIALIZE_ENUM(CandidateSetTypes,
                             {{ALPHA, "ALPHA"}, {POPMUSIC, "POPMUSIC"}});

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Param, problem_filename, tour_filename, candidate_set_type,
    initial_tour_algorithm, tree_type, move_type, subsequent_move_type,
    nonsequential_move_type, gain23_used, gain_criterion_used, runs, max_trials,
    stop_at_optimum, time_limit, total_time_limit, known_optimum,
    ascent_candidates, max_candidates, candidate_set_symmetric,
    popmusic_initial_tour, popmusic_max_neighbors, popmusic_sample_size,
    popmusic_solutions, popmusic_trials, excess, initial_period,
    initial_step_size, restricted_search, max_breadth, max_swaps, seed,
    salesmen)

Param ReadJsonParameters(const std::string &filename) {
  std::ifstream parameter_file(filename);
  if (!parameter_file.is_open())
    throw std::invalid_argument("Cannot open parameter file");

  try {
    nlohmann::json j;
    parameter_file >> j;
    Param p = j;
    PLOGI << "Json parameter file read ";
    PLOGI << "\n" << j.dump(2);
    return p;
  } catch (const nlohmann::json::parse_error &e) {
    PLOGE << "parse parameter error: " << e.what();
    throw std::invalid_argument("Error parsing parameter file: " +
                                std::string(e.what()));
  } catch (const nlohmann::json::type_error &e) {
    PLOGE << "parse parameter error: " << e.what();
    throw std::invalid_argument("Error in parameter types: " +
                                std::string(e.what()));
  }
}
