#pragma once

#include <cassert>
#include <functional>
#include <vector>

#include "ProblemType.h"
#include "type.h"

class Problem {
 private:
  std::vector<std::vector<WeightType>> costs_;

 public:
  ProblemType type = UNKNOW_PROBLEM;
  size_t dimension = 0;

 public:
  Problem() = default;
  Problem(int dimension, const std::function<WeightType(size_t, size_t)>& costs) {
    init(dimension, costs);
  }
  Problem(int dimension, const std::vector<std::vector<WeightType>> costs) {
    init(dimension, costs);
  }
  void init(int dim, const std::vector<std::vector<WeightType>>& costs) {
    dimension = dim;
    costs_ = costs;
  }
  void init(int dim, const std::function<WeightType(size_t, size_t)>& costs) {
    dimension = dim;
    costs_.resize(dimension);
    for (size_t i = 0; i < dimension; i++) {
      costs_[i].resize(dimension);
      for (size_t j = 0; j < i; j++) costs_[i][j] = costs(i, j);
    }
  }
  WeightType get(size_t i, size_t j) {
    assert(i < dimension);
    assert(j < dimension);
    return costs_[i][j];
  }
};

extern thread_local Problem problem;
