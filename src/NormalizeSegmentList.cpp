#include "data/Context.h"
#include "data/TreeNode.h"

/*
 * The NormalizeSegmentList function is used to swap the Suc and Pred fields
 * of segments in such a way that the list of segments constitutes a cyclic
 * two-way list.
 *
 * A call of the function corrupts the tree representation of the tour.
 *
 * The function is called from LinKernighan.
 */

void NormalizeSegmentList() {
  TreeNode *seg = &context.segments.front();
  TreeNode *s2;
  do {
    if (!seg->Parent->Reversed)
      s2 = seg->Suc;
    else {
      s2 = seg->Prd;
      seg->Prd = seg->Suc;
      seg->Suc = s2;
    }
  } while ((seg = s2) != &context.segments.front());
}
