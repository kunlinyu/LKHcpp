#pragma once

#include <cassert>
#include <functional>
#include <vector>

#include "ProblemType.h"
#include "type.h"

class Problem {
 private:
  std::vector<WeightType> costs_;

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
    costs_.resize(dimension * (dimension - 1) / 2);
    for (size_t i = 0; i < dimension; i++)
      for (size_t j = 0; j < i; j++) costs_[i * (i - 1) / 2 + j] = costs[i][j];
  }
  void init(int dim, const std::function<WeightType(size_t, size_t)>& costs) {
    dimension = dim;
    costs_.resize(dimension * (dimension - 1) / 2);
    for (size_t i = 0; i < dimension; i++)
      for (size_t j = 0; j < i; j++) costs_[i * (i - 1) / 2 + j] = costs(i, j);
  }
  WeightType get(size_t i, size_t j) {
    assert(i < dimension);
    assert(j < dimension);
    if (i == j) return 0;
    size_t min = std::min(i, j);
    size_t max = std::max(i, j);
    return costs_[max * (max - 1) / 2 + min];
  }
  void clear() {
    costs_.clear();
    costs_.shrink_to_fit();
  }
};

extern thread_local Problem problem;
