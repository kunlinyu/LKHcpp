// @file VariantFactory.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once
#include <memory>

#include "EdgeDataEncoder.h"
#include "NodeCoordEncoder.h"
#include "STTSPVariant.h"
#include "VariantBase.h"

class VariantFactory {
 public:
  static std::unique_ptr<VariantBase> Create(const TSPLIB& tsplib) {
    std::unique_ptr<VariantBase> result;
    if (tsplib.type == STTSP)
      result = std::unique_ptr<VariantBase>(new STTSPVariant());
    else if (not tsplib.edge_data_section.empty())
      result = std::unique_ptr<VariantBase>(new EdgeDataEncoder());
    else if (not tsplib.node_coord_section.empty())
      result = std::unique_ptr<VariantBase>(new NodeCoordEncoder());
    else if (not tsplib.edge_weight_section.empty()) {
      LOGE << "unsupported edge weight section";
      throw std::invalid_argument("unsupported edge weight section");
    } else
      throw std::invalid_argument("unknown variant");

    LOGI << "create variant with inherit chain: " << result->chain();
    return result;
  }
};