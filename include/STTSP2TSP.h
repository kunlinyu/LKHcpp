#pragma once
#include <set>
#include <vector>

#include "type.h"

std::vector<std::vector<WeightType>> STTSP2TSP(const TSPLIB& tsplib, std::vector<Node>& node_set);