// @file LKHcpp.h
// @brief
// @author Kunlin Yu
// @date 2026-04-06

#pragma once

#include "data/Param.h"
#include "data/Problem.h"
#include "data/Tour.h"

class LKHcpp {
 public:
  static Tour Solve(const Param& param, const Problem& problem);
  static Tour Solve(const std::string& param_json, const std::string& problem_tsplib);
  static void WriteTour(std::ostream& os, const TourFile& tour);
};