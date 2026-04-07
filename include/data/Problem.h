#pragma once

#include <cassert>
#include <vector>

#include "ProblemType.h"
#include "type.h"

class Problem {
 private:
  std::vector<WeightType> costs_;

 public:
  ProblemType type = UNKNOW_PROBLEM;
  int dimension = 0;

 public:
  Problem() = default;
  Problem(int dimension, ProblemType type) : dimension(dimension), type(type) {
    costs_.resize((size_t)dimension * (dimension - 1) / 2);
  }
  WeightType get(size_t i, size_t j) {
    assert(i < dimension);
    assert(j < dimension);
    size_t min = std::min(i, j);
    size_t max = std::max(i, j);
    return costs_[max * (max + 1) / 2 + min];
  }
};

extern thread_local Problem problem;
