// @file LKHcpp.h
// @brief
// @author Kunlin Yu
// @date 2026-04-07

#pragma once
#include <string>
#include <vector>
#include <limits>

#include "type.h"

struct Tour {
  std::vector<NodeIdType> node_ids;
  GainType cost = std::numeric_limits<GainType>::max();
  Tour() = default;
  Tour(const std::vector<NodeIdType>& node_ids, GainType cost) : node_ids(node_ids), cost(cost) {}
};

struct TourFile {
  std::string name;
  std::string type = "TOUR";
  std::vector<std::string> comments;
  int dimension = 0;
  std::vector<NodeIdType> node_ids;
};