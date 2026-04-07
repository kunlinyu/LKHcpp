// @file EdgeDataEncoder.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once
#include <plog/Log.h>
#include "VariantBase.h"

class EdgeDataEncoder : public VariantBase {
 public:
  Problem Encode(const TSPLIB& tsplib) override {
      std::set<NodeIdType> id_set;
      std::map<std::pair<NodeIdType, NodeIdType>, WeightType> id_2_weight_;
      for (const auto& edge_data : tsplib.edge_data_section) {
        id_set.insert(edge_data.from);
        id_set.insert(edge_data.to);
        id_2_weight_[{edge_data.from, edge_data.to}] = edge_data.weight;
      }
      if (id_set.size() != tsplib.dimension) {
        PLOGE << "count of nodes in edge data section does not match dimension";
        throw std::invalid_argument(
            "count of nodes in edge data section does not match dimension");
      }
      std::map<size_t, NodeIdType> index_2_id_;
      size_t index = 0;
      for (const auto& id : id_set) index_2_id_[index++] = id;

      Problem pb(tsplib.dimension, [&](size_t i, size_t j) {
        NodeIdType a = index_2_id_[i];
        NodeIdType b = index_2_id_[j];
        if (id_2_weight_.find({a, b}) != id_2_weight_.end())
          return id_2_weight_[{a, b}];
        return id_2_weight_[{b, a}];
      });
      return pb;
  }
};