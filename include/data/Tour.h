// @file LKHcpp.h
// @brief
// @author Kunlin Yu
// @date 2026-04-07

#pragma once
#include <limits>
#include <string>
#include <vector>

#include "type.h"

struct Tour {
  std::vector<NodeIdType> node_ids;
  GainType cost = std::numeric_limits<GainType>::max();
  Tour() = default;
  Tour(const std::vector<NodeIdType>& node_ids, GainType cost)
      : node_ids(node_ids), cost(cost) {}
};

struct TourFile {
  std::string name;
  std::string type = "TOUR";
  std::vector<std::string> comments;
  size_t dimension = 0;
  Tour tour;

  void write(std::ostream& os) const {
    os << "NAME : " << name << "\n";
    for (const auto& comment : comments) {
      os << "COMMENT : " << comment << "\n";
    }
    os << "TYPE : " << type << "\n";
    os << "DIMENSION : " << dimension << "\n";
    os << "TOUR_SECTION\n";
    for (const auto& node : tour.node_ids) {
      os << node << "\n";
    }
    os << "-1\nEOF\n";
  }
};