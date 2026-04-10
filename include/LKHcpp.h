// @file LKHcpp.h
// @brief
// @author Kunlin Yu
// @date 2026-04-06

#pragma once

#include "data/Param.h"
#include "data/Problem.h"
#include "data/TSPLIB.h"
#include "data/Tour.h"

class LKHcpp {
 public:
  static Tour Solve(const Param& param, const Problem& problem);
  static TourFile Solve(const Param& param, const TSPLIB& tsplib);
};