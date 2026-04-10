#pragma once

// for both Segment and Super Segment
class TreeNode {
 public:
  virtual ~TreeNode() = default;
  bool Reversed = false;       // Reversal bit
  TreeNode *First = nullptr;   // First child node
  TreeNode *Last = nullptr;    // Last child node
  TreeNode *Prd = nullptr;     // Predecessor in the two-way list
  TreeNode *Suc = nullptr;     // Successor in the two-way list
  TreeNode *Parent = nullptr;  // The parent node
  int Rank = 0;                // Ordinal number of the node in the list
  int Size = 0;                // Number of children
};

inline void SLink(TreeNode *a, TreeNode *b) {
  a->Suc = b;
  b->Prd = a;
}

inline void SLink(TreeNode &a, TreeNode &b) {
  a.Suc = &b;
  b.Prd = &a;
}