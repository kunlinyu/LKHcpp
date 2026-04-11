#include <cassert>

#include "Function.h"
#include "data/Context.h"
#include "data/Problem.h"

/*
 * The Flip function performs a 2-opt move. Edges (t1,t2) and (t3,t4)
 * are exchanged with edges (t2,t3) and (t4,t1). Node t4 is one of
 * t3's two neighbors on the tour; which one is uniquely determined
 * by the orientation of (t1,t2).
 *
 * The function is only used if the doubly linked list representation
 * is used for a tour; if the two-level tree representation is used,
 * the function Flip_SL is used instead.
 *
 * The 2-opt move is made by swapping Pred and Suc of each node of the
 * two segments, and then reconnecting the segments by suitable
 * settings of Pred and Suc of t1, t2, t3 and t4. In addition,
 * Rank is updated for nodes in the reversed segment (Rank gives the
 * ordinal number of a node in the tour).
 *
 * Any of two segments defined by the 2-opt move may be reversed. The
 * segment with the smallest number of nodes is reversed in order to
 * speed up computations. The number of nodes in a segment is found
 * from the Rank-values.
 *
 * The move is pushed onto a stack of 2-opt moves. The stack makes it
 * possible to undo moves (by the RestoreTour function).
 *
 * Finally, the hash value corresponding to the tour is updated.
 */

void Flip(Node *t1, Node *t2, Node *t3) {
  assert(t1->Prd == t2 || t1->Suc == t2);
  if (t3 == t2->Prd || t3 == t2->Suc) return;

  Node *t4 = t1->Suc == t2 ? t3->PrdNode() : t3->SucNode();

  if (t1->Suc != t2) {
    std::swap(t1, t2);
    std::swap(t3, t4);
  }

  // Find the segment with the smallest number of nodes
  int64_t drank = t2->Rank - t3->Rank;
  if (drank < 0) drank += context.dimension;
  if (2 * drank > (int64_t)context.dimension) {
    std::swap(t2, t3);
    std::swap(t4, t1);
  }

  // Swap segment (t3 --> t1)
  int64_t new_rank = t1->Rank;
  t1->Suc = nullptr;
  for (Node *s1 = t3; s1; s1 = s1->PrdNode()) {
    std::swap(s1->Suc, s1->Prd);
    s1->Rank = new_rank--;
  }
  Link(t3, t2);
  Link(t4, t1);
  context.swap_stack.push(t1, t2, t3, t4);
  context.hash_table.update_hash(t1->index, t2->index, t3->index, t4->index);
}

/*
   The SplitSegment function is called by the Flip_SL function to split
   a segment. Calling SplitSegment(t1,t2), where t1 and t2 are neighbors
   in the same segment, causes the segment to be split between t1 and t2.
   The smaller half is merged with its neighbouring segment, thus keeping
   the number of segments fixed.

   The implementation of SplitSegment closely follows the suggestions given in

        M. L. Fredman, D. S. Johnson & L. A. McGeoch,
        Data Structures for Traveling Salesmen",
        J. Algorithms, 16, 432-479 (1995).
 */

void SplitSegment(TreeNode *t1, TreeNode *t2) {
  TreeNode *P = t1->Parent, *Q;

  if (t2->Rank < t1->Rank) std::swap(t1, t2);
  int64_t Count = t1->Rank - P->First->Rank + 1;
  if (2 * Count < P->Size) {
    // The left part of P is merged with its neighbouring segment, Q
    Q = P->Reversed ? P->Suc : P->Prd;
    TreeNode *t = P->First->Prd;
    int64_t new_rank = t->Rank;
    if (t == Q->Last) {
      if (t == Q->First && t->Suc != P->First) {
        std::swap(t->Suc, t->Prd);
        Q->Reversed ^= 1;
        t->Reversed ^= 1;
      }
      for (t = P->First; t != t2; t = t->Suc) {
        t->Parent = Q;
        t->Rank = ++new_rank;
      }
      Q->Last = t1;
    } else {
      for (t = P->First; t != t2; t = t->Prd) {
        std::swap(t->Suc, t->Prd);
        t->Parent = Q;
        t->Rank = --new_rank;
        t->Reversed ^= 1;
      }
      Q->First = t1;
    }
    P->First = t2;
  } else {
    // The right part of P is merged with its neighbouring segment, Q
    Q = P->Reversed ? P->Prd : P->Suc;
    TreeNode *t = P->Last->Suc;
    int64_t new_rank = t->Rank;
    if (t == Q->First) {
      if (t == Q->Last && t->Prd != P->Last) {
        std::swap(t->Suc, t->Prd);
        Q->Reversed ^= 1;
        t->Reversed ^= 1;
      }
      for (t = P->Last; t != t1; t = t->Prd) {
        t->Parent = Q;
        t->Rank = --new_rank;
      }
      Q->First = t2;
    } else {
      for (t = P->Last; t != t1; t = t->Suc) {
        std::swap(t->Prd, t->Suc);
        t->Parent = Q;
        t->Rank = ++new_rank;
        t->Reversed ^= 1;
      }
      Q->Last = t2;
    }
    Count = P->Size - Count;
    P->Last = t1;
  }
  P->Size -= Count;
  Q->Size += Count;
}

#define SPLIT_CUTOFF 0.75

class ABCD {
 private:
  TreeNode *&a, *&b, *&c, *&d;

 public:
  ABCD(TreeNode *&a, TreeNode *&b, TreeNode *&c, TreeNode *&d)
      : a(a), b(b), c(c), d(d) {}
  void operator()(TreeNode *t1, TreeNode *t2, TreeNode *t3,
                  TreeNode *t4) const {
    a = t1;
    b = t2;
    c = t3;
    d = t4;
  }
};
static void FlipNodes(TreeNode *t1, TreeNode *t2, TreeNode *t3, TreeNode *t4) {
  int64_t new_rank = t4->Rank;
  t4->Suc = nullptr;
  for (TreeNode *s1 = t2; s1; s1 = s1->Prd) {
    std::swap(s1->Suc, s1->Prd);
    s1->Rank = new_rank--;
    s1->Reversed ^= 1;
  }

  t2->Suc = t3;
  (t3->Prd == t4 ? t3->Prd : t3->Suc) = t2;
  t4->Prd = t1;
  (t1->Suc == t2 ? t1->Suc : t1->Prd) = t4;

  if (t2->Parent) {
    if (t2->Parent->First == t2)
      t2->Parent->First = t4;
    else if (t4->Parent->First == t4)
      t4->Parent->First = t2;
    if (t2->Parent->Last == t2)
      t2->Parent->Last = t4;
    else if (t4->Parent->Last == t4)
      t4->Parent->Last = t2;
  }
}

/*
 * The Flip_SL function performs a 2-opt move. Edges (t1,t2) and (t3,t4)
 * are exchanged with edges (t2,t3) and (t4,t1). Node t4 is one of
 * t3's two neighbors on the tour; which one is uniquely determined
 * by the orientation of (t1,t2).
 *
 * The function is only used if the two-level tree representation is used
 * for a tour; if the doubly linked list representation is used, the function
 * Flip is used instead.
 *
 * The worst-case time cost of a 2-op move is O(n) when the doubly linked
 * list representation is used. A worst-case cost of O(sqrt(n)) per 2-opt
 * move may be achieved using the two-level tree representation.
 *
 * The idea is to divide the tour into roughly sqrt(n) segments. Each segment
 * is maintained as a doubly linked list of nodes (using pointers labeled
 * Pred and Suc). The segments are connected in a doubly linked list (using
 * pointers labeled Pred and Suc). Each segment contains a number, Rank,
 * that represents its position in the list, two pointers First and Last that
 * references the first and last node of the segment, respectively, and a bit,
 * Reversed, that is used to indicate whether the segment should be traversed
 * in forward or backward direction. Just switching this bit reverses the
 * orientation of a whole segment.
 *
 * The implementation of Flip_SL closely follows the suggestions given in
 *
 *      M. L. Fredman, D. S. Johnson & L. A. McGeoch,
 *      Data Structures for Traveling Salesmen",
 *      J. Algorithms, 16, 432-479 (1995).
 *
 * When a 2-opt move has been made it is pushed onto a stack of 2-opt moves.
 * The stack makes it possible to undo moves (by the RestoreTour function).
 *
 * Finally, the hash value corresponding to the tour is updated.
 */

void Flip_SL(Node *t1, Node *t2, Node *t3) {
  assert(t1->Prd == t2 || t1->Suc == t2);
  if (t3 == t2->Prd || t3 == t2->Suc) return;

  if (context.segments.size() == 1) {
    Flip(t1, t2, t3);
    return;
  }

  Node *t4 = t2 == SUC(t1) ? PRD(t3) : SUC(t3);
  TreeNode *P1 = nullptr, *P2 = nullptr, *P3 = nullptr, *P4 = nullptr;
  ABCD PPPP(P1, P2, P3, P4);
  PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
  // Split segments if needed
  if (P1 != P3 && P2 != P4) {
    if (P1 == P2) {
      SplitSegment(t1, t2);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
    if (P3 == P4 && P1 != P3 && P2 != P4) {
      SplitSegment(t3, t4);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
  } else if ((P1 == P3 &&
              abs(t3->Rank - t1->Rank) > SPLIT_CUTOFF * context.SegmentSize) ||
             (P2 == P4 &&
              abs(t4->Rank - t2->Rank) > SPLIT_CUTOFF * context.SegmentSize)) {
    if (P1 == P2) {
      SplitSegment(t1, t2);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
    if (P3 == P4) {
      SplitSegment(t3, t4);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
  }
  // Check if it is possible to flip locally within a segment
  TreeNode *a = nullptr, *b = nullptr, *c = nullptr, *d = nullptr;
  ABCD abcd(a, b, c, d);
  if (P1 == P3) {
    // Either the t1 --> t3 path or the t2 --> t4 path lies within one segment
    if (t1->Rank < t3->Rank) {
      if (P1 == P2 && P1 == P4 && t2->Rank > t1->Rank)
        abcd(t1, t2, t3, t4);
      else
        abcd(t2, t1, t4, t3);
    } else {
      if (P1 == P2 && P1 == P4 && t2->Rank < t1->Rank)
        abcd(t3, t4, t1, t2);
      else
        abcd(t4, t3, t2, t1);
    }
  } else if (P2 == P4) {
    // The t2 --> t4 path lies within one segment
    if (t4->Rank < t2->Rank)
      abcd(t3, t4, t1, t2);
    else
      abcd(t1, t2, t3, t4);
  }
  if (a and b and c and d) {
    // Flip locally (b --> d) within a segment
    FlipNodes(a, b, c, d);
  } else {
    // Reverse a sequence of segments
    if (P1->Suc != P2) {
      std::swap(t1, t2);
      std::swap(t3, t4);
      std::swap(P1, P2);
      std::swap(P3, P4);
    }
    // Find the sequence with the smallest number of segments
    int64_t drank;
    if ((drank = P2->Rank - P3->Rank) < 0) drank += context.segments.size();
    if (2 * drank > (int64_t)context.segments.size()) {
      std::swap(t3, t2);
      std::swap(t1, t4);
      std::swap(P3, P2);
      std::swap(P1, P4);
    }
    // Reverse the sequence of segments (P3 --> P1).  Mirrors the corresponding
    // code in the Flip function
    int64_t new_rank = P1->Rank;
    P1->Suc = nullptr;
    for (TreeNode *Q1 = P3; Q1; Q1 = Q1->Prd) {
      std::swap(Q1->Suc, Q1->Prd);
      Q1->Rank = new_rank--;
      Q1->Reversed ^= 1;
    }
    Link(P3, P2);
    Link(P4, P1);
    (t3->Suc == t4 ? t3->Suc : t3->Prd) = t2;
    (t2->Suc == t1 ? t2->Suc : t2->Prd) = t3;
    (t1->Prd == t2 ? t1->Prd : t1->Suc) = t4;
    (t4->Prd == t3 ? t4->Prd : t4->Suc) = t1;
  }
  context.swap_stack.push(t1, t2, t3, t4);
  context.hash_table.update_hash(t1->index, t2->index, t3->index, t4->index);
}

/*
 * The Flip_SSL function performs a 2-opt move. Edges (t1,t2) and (t3,t4)
 * are exchanged with edges (t2,t3) and (t4,t1). Node t4 is one of
 * t3's two neighbors on the tour; which one is uniquely determined
 * by the orientation of (t1,t2).
 *
 * The function is only used if the three-level tree representation is used
 * for a tour; if the doubly linked list representation is used, the Flip_SL
 * function is used instead.
 *
 * An average cost of O(n^(1/3)) per 2-opt move may be achieved using the
 * three-level tree representation.
 *
 * See also the documentation for the Flip_SL function.
 */

void Flip_SSL(Node *t1, Node *t2, Node *t3) {
  TreeNode *P1 = nullptr, *P2 = nullptr, *P3 = nullptr, *P4 = nullptr;
  ABCD PPPP(P1, P2, P3, P4);

  assert(t1->Prd == t2 || t1->Suc == t2);
  if (t3 == t2->Prd || t3 == t2->Suc) return;
  if (context.segments.size() == 1) {
    Flip(t1, t2, t3);
    return;
  }
  Node *t4 = t2 == SUC(t1) ? PRD(t3) : SUC(t3);
  PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
  // Split segments if needed
  if (P1 != P3 && P2 != P4) {
    if (P1 == P2) {
      SplitSegment(t1, t2);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
    if (P3 == P4 && P1 != P3 && P2 != P4) {
      SplitSegment(t3, t4);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
  } else if ((P1 == P3 &&
              abs(t3->Rank - t1->Rank) > SPLIT_CUTOFF * context.SegmentSize) ||
             (P2 == P4 &&
              abs(t4->Rank - t2->Rank) > SPLIT_CUTOFF * context.SegmentSize)) {
    if (P1 == P2) {
      SplitSegment(t1, t2);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
    if (P3 == P4) {
      SplitSegment(t3, t4);
      PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    }
  }
  // Check if it is possible to flip locally within a segment
  TreeNode *a = nullptr, *b = nullptr, *c = nullptr, *d = nullptr;
  ABCD abcd(a, b, c, d);
  if (P1 == P3) {
    // Either the t1 --> t3 path or the t2 --> t4 path lies within one segment
    if (t1->Rank < t3->Rank) {
      if (P1 == P2 && P1 == P4 && t2->Rank > t1->Rank)
        abcd(t1, t2, t3, t4);
      else
        abcd(t2, t1, t4, t3);
    } else {
      if (P1 == P2 && P1 == P4 && t2->Rank < t1->Rank)
        abcd(t3, t4, t1, t2);
      else
        abcd(t4, t3, t2, t1);
    }
  } else if (P2 == P4) {
    // The t2 --> t4 path lies within one segment
    if (t4->Rank < t2->Rank)
      abcd(t3, t4, t1, t2);
    else
      abcd(t1, t2, t3, t4);
  }
  if (a and b and c and d)
    // Flip locally (b --> d) within a segment
    FlipNodes(a, b, c, d);
  else {
    TreeNode *a = nullptr, *b = nullptr, *c = nullptr, *d = nullptr;
    TreeNode *t1 = P1, *t2 = P2, *t3 = P3, *t4 = P4;
    TreeNode *P1 = nullptr, *P2 = nullptr, *P3 = nullptr, *P4 = nullptr;
    ABCD abcd(a, b, c, d);
    ABCD PPPP(P1, P2, P3, P4);
    PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
    if (P1 != P3 && P2 != P4) {
      if (P1 == P2) {
        SplitSegment(t1, t2);
        PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
      }
      if (P3 == P4 && P1 != P3 && P2 != P4) {
        SplitSegment(t3, t4);
        PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
      }
    } else if ((P1 == P3 && abs(t3->Rank - t1->Rank) >
                                SPLIT_CUTOFF * context.SuperSegmentSize) ||
               (P2 == P4 && abs(t4->Rank - t2->Rank) >
                                SPLIT_CUTOFF * context.SuperSegmentSize)) {
      if (P1 == P2) {
        SplitSegment(t1, t2);
        PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
      }
      if (P3 == P4) {
        SplitSegment(t3, t4);
        PPPP(t1->Parent, t2->Parent, t3->Parent, t4->Parent);
      }
    }
    a = b = c = d = nullptr;
    if (P1 == P3) {
      if (t1->Rank < t3->Rank) {
        if (P1 == P2 && P1 == P4 && t2->Rank > t1->Rank)
          abcd(t1, t2, t3, t4);
        else
          abcd(t2, t1, t4, t3);
      } else {
        if (P1 == P2 && P1 == P4 && t2->Rank < t1->Rank &&
            t1->Rank - t2->Rank + 1 < (int64_t)context.segments.size())
          abcd(t3, t4, t1, t2);
        else
          abcd(t4, t3, t2, t1);
      }
    } else if (P2 == P4) {
      if (t4->Rank < t2->Rank)
        abcd(t3, t4, t1, t2);
      else
        abcd(t1, t2, t3, t4);
    }
    if (a and b and c and d)
      // Flip locally (b --> d) within a super segment
      FlipNodes(a, b, c, d);
    else {
      if (P1->Suc != P2) {
        std::swap(t1, t2);
        std::swap(t3, t4);
        std::swap(P1, P2);
        std::swap(P3, P4);
      }
      // Find the sequence with the fewest segments
      int count = P2->Rank - P3->Rank;
      if (count <= 0) count += context.super_segments.size();
      if (2 * count > (int64_t)context.super_segments.size()) {
        std::swap(t3, t2);
        std::swap(t1, t4);
        std::swap(P3, P2);
        std::swap(P1, P4);
      }
      // Reverse the sequence of segments (P3 --> P1)
      FlipNodes(P4, P3, P2, P1);
      (t3->Suc == t4 ? t3->Suc : t3->Prd) = t2;
      (t2->Suc == t1 ? t2->Suc : t2->Prd) = t3;
      (t1->Prd == t2 ? t1->Prd : t1->Suc) = t4;
      (t4->Prd == t3 ? t4->Prd : t4->Suc) = t1;
    }
  }
  if (!a and !b and !c and !d) {
    (t3->Suc == t4 ? t3->Suc : t3->Prd) = t2;
    (t2->Suc == t1 ? t2->Suc : t2->Prd) = t3;
    (t1->Prd == t2 ? t1->Prd : t1->Suc) = t4;
    (t4->Prd == t3 ? t4->Prd : t4->Suc) = t1;
  }
  context.swap_stack.push(t1, t2, t3, t4);
  context.hash_table.update_hash(t1->index, t2->index, t3->index, t4->index);
}
