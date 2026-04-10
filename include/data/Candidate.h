#pragma once
#include <climits>

// The Candidate structure is used to represent candidate edges
class Node;
struct Candidate {
  Node* To = nullptr;   // The end node of the edge
  int Cost = 0;         // Cost (distance) of the edge
  int Alpha = INT_MAX;  // Its alpha-value (Cost - Beta)

  Candidate() = default;
  Candidate(Node* To, int Cost, int Alpha) : To(To), Cost(Cost), Alpha(Alpha) {}
};
