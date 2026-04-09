// @file EdgeDataEncoder.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once
#include <plog/Log.h>

#include "VariantBase.h"

class EdgeDataEncoder : public VariantBase {
 public:
  std::string chain() const override {
    return "EdgeDataEncoder -> " + VariantBase::chain();
  }
  Problem Encode(const TSPLIB& tsplib) override {
    std::set<NodeIdType> id_set;
    for (const auto& edge_data : tsplib.edge_data_section) {
      id_set.insert(edge_data.from);
      id_set.insert(edge_data.to);
    }
    if (id_set.size() != tsplib.dimension) {
      PLOGE << "count of nodes in edge data section does not match dimension";
      throw std::invalid_argument(
          "count of nodes in edge data section does not match dimension");
    }
    std::unordered_map<NodeIdType, size_t> id_2_index;
    size_t index = 0;
    for (const auto& id : id_set)
      id_2_index[id] = index++;
    std::vector<std::vector<WeightType>> costs;
    costs.resize(tsplib.dimension);
    for (auto& cost : costs) cost.resize(tsplib.dimension);
    for (const auto& edge_data : tsplib.edge_data_section) {
      size_t i = id_2_index[edge_data.from];
      size_t j = id_2_index[edge_data.to];
      costs[i][j] = edge_data.weight;
      costs[j][i] = edge_data.weight;
    }
    return Problem(tsplib.dimension, costs);
  }
};