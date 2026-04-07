// @file TSPLIB.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "type.h"
#include "Coordinate.h"
#include "ProblemType.h"

struct EdgeData {
  NodeIdType from;
  NodeIdType to;
  WeightType weight;
};

enum EdgeWeightTypes {
  EXPLICIT,
  ATT,
  EUC_2D,
  EUC_3D,
  MAX_2D,
  MAX_3D,
  MAN_2D,
  MAN_3D,
  CEIL_2D,
  CEIL_3D,
  GEO,
  GEOM,
  GEO_MEEUS,
  GEOM_MEEUS,
  XRAY1,
  XRAY2,
  SPECIAL
};

enum EdgeWeightFormat {
  UNSET_FORMAT,
  FUNCTION,
  FULL_MATRIX,
  UPPER_ROW,
  LOWER_ROW,
  UPPER_DIAG_ROW,
  LOWER_DIAG_ROW,
  UPPER_COL,
  LOWER_COL,
  UPPER_DIAG_COL,
  LOWER_DIAG_COL
};

enum CoordTypes { TWOD_COORDS, THREED_COORDS, NO_COORDS };

enum DisplayDataType {
  COORD_DISPLAY,
  TWOD_DISPLAY,
  NO_DISPLAY,
};

enum EdgeDataFormat {
  NO_FORMAT,
  EDGE_LIST,
  ADJ_LIST,
};

const int DepotIndex = 1;

struct TSPLIB {
  std::string name;
  std::vector<std::string> comments;
  ProblemType type = UNKNOW_PROBLEM;
  int dimension = 0;
  CapacityType capacity = 0;

  EdgeWeightTypes edge_weight_type = EXPLICIT;
  EdgeWeightFormat edge_weight_format = UNSET_FORMAT;
  EdgeDataFormat edge_data_format = EDGE_LIST;
  CoordTypes node_coord_type = NO_COORDS;
  DisplayDataType display_data_type = COORD_DISPLAY;

  std::map<NodeIdType, Coordinate> node_coord_section;
  std::vector<NodeIdType> depot_section;
  std::map<NodeIdType, std::vector<DemandType>> demand_section;
  std::vector<EdgeData> edge_data_section;
  std::vector<std::pair<NodeIdType, NodeIdType>> fixed_edges_section;
  std::map<NodeIdType, Coordinate> display_data_section;
  std::vector<NodeIdType> tour_section;
  std::vector<std::vector<WeightType>> edge_weight_section;

  std::set<NodeIdType> required_nodes_section;

  bool IsAsymmetric() const { return type == ATSP; }
};