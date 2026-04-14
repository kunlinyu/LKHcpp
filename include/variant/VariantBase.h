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
  virtual std::string chain() const { return "VariantBase"; }
  virtual Problem Encode(const TSPLIB& tsplib) = 0;
  virtual Tour Decode(const Tour& tour) { return tour; };
  virtual GainType Penalty() { return 0; }
  virtual Tour Repair(const Tour& tour) { return tour; }
};