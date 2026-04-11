#pragma once
#include <climits>

#include "type.h"

// The Candidate structure is used to represent candidate edges
class Node;
struct Candidate {
  Node* To = nullptr;   // The end node of the edge
  WeightType Cost = 0;  // Cost (distance) of the edge
  WeightType Alpha = INT_MAX;  // Its alpha-value (Cost - Beta)

  Candidate() = default;
  Candidate(Node* To, WeightType Cost, WeightType Alpha)
      : To(To), Cost(Cost), Alpha(Alpha) {}
};
