#pragma once

#include <cassert>
#include <functional>
#include <vector>

#include "ProblemType.h"
#include "plog/Log.h"
#include "type.h"

class Problem {
 private:
  std::vector<std::vector<WeightType>> costs_;

 public:
  ProblemType type = UNKNOW_PROBLEM;
  const std::vector<std::vector<WeightType>>& costs() const { return costs_; }

 public:
  Problem() = default;
  explicit Problem(const std::vector<std::vector<WeightType>>& costs) {
    costs_ = costs;
  }
  Problem(size_t dimension, const std::function<WeightType(size_t, size_t)>& costs) {
    costs_.resize(dimension);
    for (size_t i = 0; i < dimension; i++) {
      costs_[i].resize(dimension);
      for (size_t j = 0; j < i; j++) costs_[i][j] = costs_[j][i] = costs(i, j);
    }
  }
  size_t dimension() const { return costs_.size(); }
  WeightType get(size_t i, size_t j) const {
    assert(i < dimension);
    assert(j < dimension);
    return costs_[i][j];
  }
};

