#pragma once
#include <set>
#include <vector>

#include "type.h"

std::vector<std::vector<int>> STTSP2TSP(const std::set<NodeIdType> &required);