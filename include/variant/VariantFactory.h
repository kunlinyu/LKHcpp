// @file VariantFactory.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once
#include <memory>

#include "EdgeDataEncoder.h"
#include "NodeCoordEncoder.h"
#include "VariantBase.h"

class VariantFactory {
 public:
  static std::unique_ptr<VariantBase> CreateVariant(const TSPLIB& tsplib) {
    if (not tsplib.edge_data_section.empty())
      return std::unique_ptr<VariantBase>(new EdgeDataEncoder());
    if (not tsplib.node_coord_section.empty())
      return std::unique_ptr<VariantBase>(new NodeCoordEncoder());
    if (not tsplib.edge_weight_section.empty()) {
      PLOGE << "unsupported edge weight section";
      throw std::invalid_argument("unsupported edge weight section");
    }
    throw std::invalid_argument("unknown variant");
  }
};