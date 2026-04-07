#include <climits>
#include <functional>

#include "SwapStack.h"
#include "data/Candidate.h"
#include "data/Context.h"

/*
 * The StoreTour function is called each time the tour has been improved by
 * the LinKernighan function.
 *
 * The function "activates" all nodes involved in the current sequence of moves.
 *
 * It sets OldPred to Pred and OldSuc to Suc for each of these nodes. In this
 * way it can always be determined whether an edge belongs to current starting
 * tour. This is used by the BestMove function to determine whether an edge is
 * excludable.
 *
 * Finally, for each of these nodes the function updates their Cost field.
 * The Cost field contains for each node its minimum cost of candidate edges
 * not on the tour. The value is used by the BestMove function to decide
 * whether a tentative non-gainful move should be considered.
 */

void StoreTour(const std::function<void(Node *)> &enqueue) {
  while (not context.swap_stack.empty()) {
    SwapRecord rec = context.swap_stack.pop();
    for (int i = 1; i <= 4; i++) {
      Node *t = rec.get(i);
      enqueue(t);
      t->OldPrd = t->PrdNode();
      t->OldSuc = t->SucNode();
      t->ResetExcluded();
      t->Cost = INT_MAX;
      Node *u;
      for (const auto &Nt : t->candidates) {
        u = Nt->To;
        if (u != t->Prd && u != t->Suc && Nt->Cost < t->Cost)
          t->Cost = Nt->Cost;
      }
    }
  }
}