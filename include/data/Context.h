#pragma once

#include <limits>

#include "HashTable.h"
#include "Problem.h"
#include "SwapStack.h"
#include "TreeNode.h"
#include "type.h"

class Node;

typedef Node *(*MoveFunction)(Node *t1, Node *t2, GainType *G0, GainType *Gain);

struct Context {
  GainType BetterCost;  // Cost of the tour stored in BetterTour
  std::vector<NodeIdType>
      BetterTour;  // Table containing the currently best tour in a run

  Node *FirstNode;  // First node in the list of nodes

  std::vector<TreeNode> segments;  // Array of all segments
  std::vector<TreeNode> super_segments;

  size_t SegmentSize;       // Desired not count of each segment
  size_t SuperSegmentSize;  // Desired segment count of each super segment

  HashTable hash_table;  // Hash table used for storing tours

  std::vector<Node> node_set;  // Array of all nodes
  int Norm;                    // Measure of a 1-tree's discrepancy from a tour

  GainType Optimum =
      std::numeric_limits<GainType>::min();  // If StopAtOptimum is 1, a
                                             // run will be terminated as soon
                                             // as a tour length becomes equal
                                             // this value

  std::vector<std::vector<WeightType>>
      CostMatrix;  // Cost matrix of the problem
  std::vector<std::vector<WeightType>>
      DMatrix;  // CostMatrix + Nodes' Pi values

  bool Reversed =
      false;  // Boolean used to indicate whether a tour has been reversed

  SwapStack swap_stack;

  int Trial;  // Ordinal number of the current trial

  double StartTime;  // Time when execution starts

  MoveFunction BestMove;
  MoveFunction BestSubsequentMove;

  std::function<WeightType(const Node *, const Node *)> C =
      [this](const Node *Na, const Node *Nb) { return C_EXPLICIT(Na, Nb); };
  std::function<WeightType(const Node *, const Node *)> D =
      [this](const Node *Na, const Node *Nb) { return D_EXPLICIT(Na, Nb); };

  WeightType C_EXPLICIT(const Node *Na, const Node *Nb) {
    return CostMatrix[Na->index][Nb->index];
  }
  WeightType D_EXPLICIT(const Node *Na, const Node *Nb) {
    return C_EXPLICIT(Na, Nb) + Na->Pi + Nb->Pi;
  }

  void SwitchCostToD() {
    DMatrix = CostMatrix;
    for (size_t i = 0; i < node_set.size(); i++)
      for (size_t j = 0; j < node_set.size(); j++)
        DMatrix[i][j] += node_set[i].Pi + node_set[j].Pi;
    C = [this](const Node *Na, const Node *Nb) {
      return DMatrix[Na->index][Nb->index];
    };
  }

  Node *gain23_s1 = nullptr;
  bool gain23_old_reversed = false;

  size_t dimension;
  ~Context() {
    LOGD << "Clear CostMatrix";
    CostMatrix.clear();
    LOGD << "Clear DMatrix";
    DMatrix.clear();
  }
};

extern thread_local Context context;
