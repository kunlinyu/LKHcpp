#pragma once

#include <limits>

#include "HashTable.h"
#include "Problem.h"
#include "SwapStack.h"
#include "TreeNode.h"
#include "type.h"

class Node;

typedef Node *(*MoveFunction)(Node *t1, Node *t2, GainType *G0, GainType *Gain);
typedef int (*CostFunction)(const Node *Na, const Node *Nb);

struct Context {
  GainType BetterCost;  // Cost of the tour stored in BetterTour
  std::vector<NodeIdType>
      BetterTour;  // Table containing the currently best tour in a run

  Node *FirstNode;  // First node in the list of nodes

  std::vector<TreeNode> segments;  // Array of all segments
  std::vector<TreeNode> super_segments;

  int SegmentSize;       // Desired not count of each segment
  int SuperSegmentSize;  // Desired segment count of each super segment

  HashTable hash_table;  // Hash table used for storing tours

  std::vector<Node> node_set;  // Array of all nodes
  int Norm;                    // Measure of a 1-tree's discrepancy from a tour

  GainType Optimum =
      std::numeric_limits<GainType>::min();  // If StopAtOptimum is 1, a
                                             // run will be terminated as soon
                                             // as a tour length becomes equal
                                             // this value

  std::vector<std::vector<WeightType>> CostMatrix;  // Cost matrix of the problem
  std::vector<std::vector<WeightType>> DMatrix;     // CostMatrix + Nodes' Pi values

  bool Reversed =
      false;  // Boolean used to indicate whether a tour has been reversed

  SwapStack swap_stack;

  int Trial;  // Ordinal number of the current trial

  double StartTime;  // Time when execution starts

  MoveFunction BestMove, BestSubsequentMove;

  std::function<int(const Node *, const Node *)> C =
      [this](const Node *Na, const Node *Nb) { return C_EXPLICIT(Na, Nb); };
  std::function<int(const Node *, const Node *)> D =
      [this](const Node *Na, const Node *Nb) { return D_EXPLICIT(Na, Nb); };

  WeightType C_EXPLICIT(const Node *Na, const Node *Nb) {
    return CostMatrix[Na->index][Nb->index];
  }
  WeightType D_EXPLICIT(const Node *Na, const Node *Nb) {
    return C_EXPLICIT(Na, Nb) + Na->Pi + Nb->Pi;
  }

  void SwitchCostToD() {
    DMatrix = CostMatrix;
    for (int i = 0; i < node_set.size(); i++)
      for (int j = 0; j < node_set.size(); j++)
        DMatrix[i][j] += node_set[i].Pi + node_set[j].Pi;
    C = [this](const Node *Na, const Node *Nb) {
      return DMatrix[Na->index][Nb->index];
    };
  }

  Node *gain23_s1 = nullptr;
  bool gain23_old_reversed = false;

  Problem problem;
};

extern Context context;
