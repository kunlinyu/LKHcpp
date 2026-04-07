// @file TSPLIBVariant.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once

#include <plog/Log.h>

#include "Distance.h"
#include "VariantBase.h"

class TSPLIBVariant : public VariantBase {
 public:
  Problem Encode(TSPLIB& tsplib) override {
    if (not tsplib.edge_data_section.empty()) {
      return EncodeEdgeDataSection(tsplib.dimension, tsplib.edge_data_section);
    }
    if (not tsplib.edge_weight_section.empty()) {
      PLOGE << "unsported edge weight section";
      throw std::invalid_argument("unsported edge weight section");
    }
    if (not tsplib.node_coord_section.empty()) {
      return EncodeNodeCoordSection(tsplib.dimension, tsplib.edge_weight_type,
                                    tsplib.node_coord_section);
    }
    throw std::invalid_argument(
        "no edge data section, edge weight section or node coord section");
  }

 private:
  static Problem EncodeEdgeDataSection(
      size_t dimension, const std::vector<EdgeData>& edge_data_section) {
    std::set<NodeIdType> id_set;
    std::map<std::pair<NodeIdType, NodeIdType>, WeightType> id_2_weight_;
    for (const auto& edge_data : edge_data_section) {
      id_set.insert(edge_data.from);
      id_set.insert(edge_data.to);
      id_2_weight_[{edge_data.from, edge_data.to}] = edge_data.weight;
    }
    if (id_set.size() != dimension) {
      PLOGE << "count of nodes in edge data section does not match dimension";
      throw std::invalid_argument(
          "count of nodes in edge data section does not match dimension");
    }
    std::map<size_t, NodeIdType> index_2_id_;
    size_t index = 0;
    for (const auto& id : id_set) index_2_id_[index++] = id;

    Problem pb(dimension, [&](size_t i, size_t j) {
      NodeIdType a = index_2_id_[i];
      NodeIdType b = index_2_id_[j];
      if (id_2_weight_.find({a, b}) != id_2_weight_.end())
        return id_2_weight_[{a, b}];
      return id_2_weight_[{b, a}];
    });
    return pb;
  }

  static Problem EncodeNodeCoordSection(
      size_t dimension, EdgeWeightTypes edge_weight_type,
      std::map<NodeIdType, Coordinate>& edge_weight_section) {
    int (*Distance)(const Coordinate* Na, const Coordinate* Nb) = nullptr;
    switch (edge_weight_type) {
      case EXPLICIT:
        PLOGE << "explicit weight type should not contains NodeCoordSection";
        throw std::invalid_argument(
            "explicit weight type should not contains NodeCoordSection");
        break;
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
        PLOGE << "Unsupported edge weight type: " << edge_weight_type;
        throw std::invalid_argument("Unsupported edge weight type: " +
                                    std::to_string(edge_weight_type));
    }

    std::map<size_t, NodeIdType> index_2_id_;
    size_t index = 0;
    for (const auto& pair : edge_weight_section)
      index_2_id_[index++] = pair.first;

    Problem pb(dimension, [&](size_t i, size_t j) {
      const Coordinate& coord_i = edge_weight_section.at(index_2_id_[i]);
      const Coordinate& coord_j = edge_weight_section.at(index_2_id_[j]);
      return Distance(&coord_i, &coord_j);
    });
    return pb;
  }
};