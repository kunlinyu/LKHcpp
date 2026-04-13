// @file LKHcpp.h
// @brief
// @author Kunlin Yu
// @date 2026-04-06

#pragma once

#include <atomic>

#include "data/Param.h"
#include "data/Problem.h"
#include "data/TSPLIB.h"
#include "data/Tour.h"

class LKHcpp {
 private:
  std::atomic<bool> stop_{false};

 public:
  Tour Solve(const Param& param, const Problem& problem);

  Tour SolveParallel(const Param& param, const Problem& problem);

  TourFile Solve(const Param& param, const TSPLIB& tsplib);

  void Solve(std::istream& param_json, std::istream& tsplib_text,
             std::ostream& tour_text);

  const char* CSolve(const char* param_json, const char* tsplib_text,
                     size_t* tour_len);

  void stop() { stop_.store(true, std::memory_order_release); }
};
