#pragma once

#include <limits>
#include <list>

#include "HashTable.h"
#include "SwapStack.h"
#include "TreeNode.h"
#include "type.h"
struct Node;

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

  std::vector<Node> NodeSet;  // Array of all nodes
  int Norm;                   // Measure of a 1-tree's discrepancy from a tour

  GainType Optimum =
      std::numeric_limits<GainType>::min();  // If StopAtOptimum is 1, a
                                             // run will be terminated as soon
                                             // as a tour length becomes equal
                                             // this value

  std::vector<std::vector<int>> CostMatrix;  // Cost matrix of the problem

  bool Reversed =
      false;  // Boolean used to indicate whether a tour has been reversed

  SwapStack swap_stack;

  int Trial;  // Ordinal number of the current trial

  double StartTime;  // Time when execution starts

  MoveFunction BestMove, BestSubsequentMove;

  static int C(const Node* Na, const Node* Nb) {
    if (Na->Id < Nb->Id)
      return Nb->C[Na->Id];
    else
      return Na->C[Nb->Id];
  }
  static int D(const Node* Na, const Node* Nb) {
    return C(Na, Nb) + Na->Pi + Nb->Pi;
  }

  Node *gain23_s1 = nullptr;
  bool gain23_old_reversed = false;
};

extern Context context;
