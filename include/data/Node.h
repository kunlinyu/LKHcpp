#pragma once
#include <vector>

#include "Coordinate.h"
#include "TreeNode.h"
#include "candidate/CandidateSet.h"
struct Segment;

class Node : public TreeNode, public Coordinate {
 public:
  size_t index;
  NodeIdType Id;   // Number of the node (1...Dimension)
  NodeIdType
      OriginalId;  // The original Id in a SDVRP or STTSPinstance (REMOVE)

  WeightType Cost;      // "Best" cost of an edge emanating from the node
  WeightType NextCost;  // During the ascent, the next best cost of an edge
                        // emanating
                 // from the node

  WeightType Pi;  // Pi-value of the node

  Node *SucNode() const { return dynamic_cast<Node *>(Suc); }
  Node *PrdNode() const { return dynamic_cast<Node *>(Prd); }
  static Node *MoveSuc(Node *&node) {
    node = node->SucNode();
    return node;
  }

  Node *OldPrd;       // Previous values of Pred
  Node *OldSuc;       // Previous values of Suc
  Node *BestSuc;      // Best successor node in the currently best tour(REMOVE)
  Node *NextBestSuc;  // Next best successor node in the currently best
                      // tour(REMOVE)

  Node *Dad;  // Father of the node in the minimum 1-tree

  Node *Next;  // Auxiliary pointer, usually to the next node in a list of nodes
               // (e.g., the list of "active" nodes)(REMOVE)
  Node *Prev;  // Auxiliary pointer, usually to the previous node in a list of
               // nodes(REMOVE)

  // Booleans used for indicating whether one (or both) of the adjoining nodes
  // on the old tour has been excluded
  bool OldPredExcluded;  // (REMOVE)
  bool OldSucExcluded;   // (REMOVE)
  void ResetExcluded() { OldPredExcluded = OldSucExcluded = false; }

  std::vector<std::vector<int>>
      Paths;  // Paths[i][j] is the j'th node on the path from this node to its
              // i'th nearest neighbor(REMOVE)

  CandidateSet candidates;
};
// The Node structure is used to represent nodes (cities) of the problem

inline void Link(TreeNode *a, TreeNode *b) {
  a->Suc = b;
  b->Prd = a;
}

inline void Link(TreeNode& a, TreeNode& b) {
  a.Suc = &b;
  b.Prd = &a;
}

inline bool InBestTour(Node *a, Node *b) {
  return a->BestSuc == b || b->BestSuc == a;
}

inline bool Near(Node *a, Node *b) {
  return a->BestSuc ? InBestTour(a, b) : a->Dad == b || b->Dad == a;
}

inline bool InNextBestTour(Node *a, Node *b) {
  return a->NextBestSuc == b || b->NextBestSuc == a;
}

inline void Follow(TreeNode *b, TreeNode *a) {
  Link(b->Prd, b->Suc);
  Link(b, b);
  Link(b, a->Suc);
  Link(a, b);
}

inline void Precede(Node *a, Node *b) {
  Link(a->Prd, a->Suc);
  Link(a, a);
  Link(b->Prd, a);
  Link(a, b);
}
