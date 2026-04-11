#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"

/*
 * The SegmentSize function returns the number of nodes in the
 * tour segment between two given nodes in the current direction.
 * Note, however, that if the two-level or three-level tree is used,
 * the number of nodes is only approximate (for efficiency reasons).
 *
 * Time complexity: O(1).
 */

size_t SegmentSize1(Node *ta, Node *tb) {
  int n = !context.Reversed ? tb->Rank - ta->Rank : ta->Rank - tb->Rank;
  return (n < 0 ? n + context.dimension : n) + 1;
}

size_t SegmentSize2(Node *ta, Node *tb) {
  TreeNode *Pa, *Pb;
  int nLeft, nMid, nRight;

  Pa = ta->Parent;
  Pb = tb->Parent;
  if (Pa == Pb) {
    int n = context.Reversed == Pa->Reversed ? tb->Rank - ta->Rank
                                             : ta->Rank - tb->Rank;
    return (n < 0 ? n + context.dimension : n) + 1;
  }
  nLeft = context.Reversed == Pa->Reversed ? Pa->Last->Rank - ta->Rank
                                           : ta->Rank - Pa->First->Rank;
  if (nLeft < 0) nLeft += Pa->Size;
  nMid = !context.Reversed ? Pb->Rank - Pa->Rank : Pa->Rank - Pb->Rank;
  if (nMid < 0) nMid += context.segments.size();
  nMid = nMid == 2 ? (!context.Reversed ? Pa->Suc : Pa->Prd)->Size
                   : (nMid - 1) * context.SegmentSize;
  nRight = context.Reversed == Pb->Reversed ? tb->Rank - Pb->First->Rank
                                            : Pb->Last->Rank - tb->Rank;
  if (nRight < 0) nRight += Pb->Size;
  return nLeft + nMid + nRight + 2;
}

size_t SegmentSize3(Node *ta, Node *tb) {
  TreeNode *Pa, *Pb;
  TreeNode *PPa, *PPb;
  int n, nLeft, nMid, nRight;

  Pa = ta->Parent;
  Pb = tb->Parent;
  PPa = Pa->Parent;
  PPb = Pb->Parent;
  if (Pa == Pb) {
    n = context.Reversed == (Pa->Reversed != PPa->Reversed)
            ? tb->Rank - ta->Rank
            : ta->Rank - tb->Rank;
    if (n < 0) n += context.dimension;
  } else if (PPa == PPb) {
    nLeft = context.Reversed == (Pa->Reversed != PPa->Reversed)
                ? Pa->Last->Rank - ta->Rank
                : ta->Rank - Pa->First->Rank;
    if (nLeft < 0) nLeft += Pa->Size;
    nMid = context.Reversed == PPa->Reversed ? Pb->Rank - Pa->Rank
                                             : Pa->Rank - Pb->Rank;
    if (nMid < 0) nMid += context.segments.size();
    nMid = nMid == 2
               ? (context.Reversed == PPa->Reversed ? Pa->Suc : Pa->Prd)->Size
               : (nMid - 1) * context.SegmentSize;
    nRight =
        (context.Reversed != PPa->Reversed) == (Pb->Reversed != PPb->Reversed)
            ? tb->Rank - Pb->First->Rank
            : Pb->Last->Rank - tb->Rank;
    if (nRight < 0) nRight += Pb->Size;
    n = nLeft + nMid + nRight + 1;
  } else {
    nLeft = context.Reversed == PPa->Reversed ? PPa->Last->Rank - Pa->Rank
                                              : Pa->Rank - PPa->First->Rank;
    if (nLeft < 0) nLeft += PPa->Size;
    if (nLeft > 0)
      nLeft *= context.SegmentSize;
    else {
      nLeft = context.Reversed == (Pa->Reversed != PPa->Reversed)
                  ? Pa->Last->Rank - ta->Rank
                  : ta->Rank - Pa->First->Rank;
      if (nLeft < 0) nLeft += Pa->Size;
    }
    nMid = !context.Reversed ? PPb->Rank - PPa->Rank : PPa->Rank - PPb->Rank;
    if (nMid < 0) nMid += context.super_segments.size();
    nMid = nMid == 2 ? (!context.Reversed ? PPa->Suc : PPa->Prd)->Size
                     : (nMid - 1) * context.SuperSegmentSize;
    nMid *= context.SegmentSize;
    nRight = context.Reversed == PPb->Reversed ? Pb->Rank - PPb->First->Rank
                                               : PPb->Last->Rank - Pb->Rank;
    if (nRight < 0) nRight += PPb->Size;
    if (nRight > 0)
      nRight *= context.SegmentSize;
    else {
      nRight =
          (context.Reversed != PPa->Reversed) == (Pb->Reversed != PPb->Reversed)
              ? tb->Rank - Pb->First->Rank
              : Pb->Last->Rank - tb->Rank;
      if (nRight < 0) nRight += Pb->Size;
    }
    n = nLeft + nMid + nRight + 1;
  }
  return n + 1;
}

size_t SegmentSize(Node *ta, Node *tb) {
  switch (param.tree_type) {
    case 1:
      return SegmentSize1(ta, tb);
    default:
    case 2:
      return SegmentSize2(ta, tb);
    case 3:
      return SegmentSize3(ta, tb);
  }
}
