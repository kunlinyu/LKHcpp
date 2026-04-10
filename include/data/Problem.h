#pragma once

#include <plog/Log.h>

#include <cassert>
#include <functional>
#include <vector>

#include "type.h"

class Problem {
 private:
  std::vector<std::vector<WeightType>> costs_;

 public:
  Problem() = default;
  explicit Problem(const std::vector<std::vector<WeightType>>& costs) {
    costs_ = costs;
  }
  Problem(size_t dimension,
          const std::function<WeightType(size_t, size_t)>& costs) {
    costs_.resize(dimension);
    for (size_t i = 0; i < dimension; i++) {
      costs_[i].resize(dimension);
      for (size_t j = 0; j < i; j++) costs_[i][j] = costs_[j][i] = costs(i, j);
    }
  }

  size_t dimension() const { return costs_.size(); }

  const std::vector<std::vector<WeightType>>& costs() const { return costs_; }

  WeightType get(size_t i, size_t j) const {
    assert(i < costs_.size());
    assert(j < costs_[i].size());
    return costs_[i][j];
  }
};
