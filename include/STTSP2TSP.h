#pragma once
#include <set>
#include <vector>

#include "type.h"

std::vector<std::vector<WeightType>> STTSP2TSP(const std::set<NodeIdType> &required, std::vector<EdgeData> edge_data_section);