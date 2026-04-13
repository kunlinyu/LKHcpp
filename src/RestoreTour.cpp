#include "Tree.h"

/*
 * The RestoreTour function is used to undo a series of moves. The function
 * restores the tour from SwapStack, the stack of 2-opt moves. A bad sequence
 * of moves is undone by unstacking the 2-opt moves and making the inverse
 * 2-opt moves in this reversed sequence.
 */

void RestoreTour() {
  bool odd = true;
  context.swap_stack.restore([&odd](SwapRecord& record) {
    if (odd) {
      Swap1(record.T3(), record.T2(), record.T1());
      record.ResetExcluded();
    }
    odd = !odd;
  });
}