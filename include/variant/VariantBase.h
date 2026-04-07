// @file VariantBase.h
// @brief base class of variant TSP(symmetric, asymmetric, etc.)
// @author Kunlin Yu
// @date 2026/4/7

#pragma once
#include "data/Problem.h"
#include "data/TSPLIB.h"
#include "data/Tour.h"

class VariantBase {
 public:
  virtual ~VariantBase() = default;
  virtual Problem Encode(TSPLIB& tsplib) = 0;
  virtual Tour Decode(const Tour& tour) { return tour; };
  virtual GainType Penalty() { return 0; }
};