// @file Tour.h
// @brief
// @author Kunlin Yu
// @date 2026/4/13

#pragma once
#include <vector>

#include "data/Node.h"
#include "data/Tour.h"
#include "type.h"

void RecordBetterTour(std::vector<NodeIdType>& BetterTour, Node* FirstNode);
void RestoreTour();
Tour ExtractFinalTour(const std::vector<NodeIdType>& ids);