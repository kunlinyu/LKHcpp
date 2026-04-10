// @file ParamReader.h
// @brief
// @author Kunlin Yu
// @date 2026-04-06

#pragma once

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
    Param, tsplib_filename, tour_filename, candidate_set_type,
    initial_tour_algorithm, tree_type, move_type, subsequent_move_type,
    nonsequential_move_type, gain23_used, gain_criterion_used, runs, max_trials,
    stop_at_optimum, time_limit, total_time_limit, known_optimum,
    ascent_candidates, max_candidates, candidate_set_symmetric,
    popmusic_initial_tour, popmusic_max_neighbors, popmusic_sample_size,
    popmusic_solutions, popmusic_trials, excess, initial_period,
    initial_step_size, restricted_search, max_breadth, max_swaps, seed,
    salesmen)

class ParamReader {
 private:
  template <typename ParseFunc>
  static Param Read(ParseFunc parse) {
    try {
      nlohmann::json j = parse();
      Param p = j;
      LOGI << "Json parameter read:";
      LOGI << "\n" << j.dump(2);
      return p;
    } catch (const nlohmann::json::parse_error& e) {
      LOGE << "parse parameter error: " << e.what();
      throw std::invalid_argument("Error parsing parameter file: " +
                                  std::string(e.what()));
    } catch (const nlohmann::json::type_error& e) {
      LOGE << "parse parameter error: " << e.what();
      throw std::invalid_argument("Error in parameter types: " +
                                  std::string(e.what()));
    }
  }

 public:
  static Param ReadString(const std::string& param_json) {
    return Read([&]() { return nlohmann::json::parse(param_json); });
  }

  static Param ReadStream(std::istream& is) {
    return Read([&]() {
      nlohmann::json j;
      is >> j;
      return j;
    });
  }
};
