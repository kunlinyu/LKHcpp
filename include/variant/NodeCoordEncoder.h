// @file NodeCoordEncoder.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once
#include <plog/Log.h>

#include <stdexcept>

#include "Distance.h"
#include "VariantBase.h"

class NodeCoordEncoder : public VariantBase {
 public:
  std::string chain() const override {
    return "NodeCoordEncoder -> " + VariantBase::chain();
  }
  Problem Encode(const TSPLIB& tsplib) override {
    int (*Distance)(const Coordinate* Na, const Coordinate* Nb) = nullptr;
    switch (tsplib.edge_weight_type) {
      case EXPLICIT:
        PLOGE << "explicit weight type should not contains NodeCoordSection";
        throw std::invalid_argument(
            "explicit weight type should not contains NodeCoordSection");
        // clang-format off
      case ATT: Distance = Distance_ATT; break;
      case EUC_2D: Distance = Distance_EUC_2D; break;
      case EUC_3D: Distance = Distance_EUC_3D; break;
      case MAX_2D: Distance = Distance_MAX_2D; break;
      case MAX_3D: Distance = Distance_MAN_3D; break;
      case MAN_2D: Distance = Distance_MAN_2D; break;
      case MAN_3D: Distance = Distance_MAN_3D; break;
      case CEIL_2D: Distance = Distance_CEIL_2D; break;
      case CEIL_3D: Distance = Distance_CEIL_3D; break;
      case GEO: Distance = Distance_GEO; break;
      case GEOM: Distance = Distance_GEOM; break;
      case GEO_MEEUS: Distance = Distance_GEO_MEEUS; break;
      case GEOM_MEEUS: Distance = Distance_GEOM_MEEUS; break;
      case XRAY1: Distance = Distance_XRAY1; break;
      case XRAY2: Distance = Distance_XRAY2; break;
        // clang-format on
      default:
        PLOGE << "Unsupported edge weight type: " << tsplib.edge_weight_type;
        throw std::invalid_argument("Unsupported edge weight type: " +
                                    std::to_string(tsplib.edge_weight_type));
    }

    std::map<size_t, NodeIdType> index_2_id_;
    size_t index = 0;
    for (const auto& pair : tsplib.node_coord_section)
      index_2_id_[index++] = pair.first;

    Problem pb(tsplib.dimension, [&](size_t i, size_t j) {
      const Coordinate& coord_i = tsplib.node_coord_section.at(index_2_id_[i]);
      const Coordinate& coord_j = tsplib.node_coord_section.at(index_2_id_[j]);
      return Distance(&coord_i, &coord_j);
    });
    return pb;
  }
};