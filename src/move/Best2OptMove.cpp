#include <limits>

#include "Exclude.h"
#include "LKH.h"
#include "Tree.h"
#include "data/Candidate.h"
#include "data/Param.h"
#include "data/Problem.h"

/*
 * The Best2OptMove function makes sequential edge exchanges. If possible, it
 * makes a 2-opt move that improves the tour. Otherwise, it makes the most
 * promising 2-opt move that fulfils the positive gain criterion. To prevent
 * an infinity chain of moves the last edge in a 2-opt move must not previously
 * have been included in the chain.
 *
 * The edge (t1,t2) is the first edge to be exchanged. G0 is a pointer to the
 * accumulated gain.
 *
 * In case a 2-opt move is found that improves the tour, the improvement of
 * the cost is made available to the caller through the parameter Gain.
 * If *Gain > 0, an improvement of the current tour has been found. In this
 * case the function returns nullptr.
 *
 * Otherwise, the best 2-opt move is made, and a pointer to the node that was
 * connected to t1 (in order to close the tour) is returned. The new
 * accumulated gain is made available to the caller through the parameter G0.
 *
 * The function is called from the LinKernighan function.
 */

Node *Best2OptMove(Node *t1, Node *t2, GainType *G0, GainType *Gain) {
  context.swap_stack.set_check_point();
  if (SUC(t1) != t2) context.Reversed = !context.Reversed;

  Node *T3 = nullptr;
  Node *T4 = nullptr;
  GainType BestG2 = std::numeric_limits<GainType>::min();
  int Breadth2 = 0;

  /*
   * Determine (T3,T4) = (t3,t4)
   * such that
   *
   *     G4 = *G0 - C(t2,T3) + C(T3,T4)
   *
   * is maximum (= BestG2), and (T3,T4) has not previously been included.
   * If during this process a legal move with *Gain > 0 is found, then make
   * the move and exit Best2OptMove immediately
   */

  // Choose (t2,t3) as a candidate edge emanating from t2
  for (const auto &c2 : t2->candidates) {
    Candidate *Nt2 = c2;
    Node *t3 = Nt2->To;
    GainType G1 = *G0 - Nt2->Cost;
    if (t3 == t2->Prd || t3 == t2->Suc ||
        (G1 <= 0 && param.gain_criterion_used))
      continue;
    // Choose t4 (only one choice gives a closed tour)
    Node *t4 = PRD(t3);
    GainType G2 = G1 + context.C(t3, t4);
    if (t4 != t1) {
      *Gain = G2 - context.C(t4, t1);
      if (*Gain > 0) {
        Swap1(t1, t2, t3);
        if (Improvement(Gain, t1, t2)) return nullptr;
      }
    }
    if (++Breadth2 > param.max_breadth) break;
    if (param.gain_criterion_used && G2 - 1 < t4->Cost) continue;
    if ((G2 > BestG2 || (G2 == BestG2 && !Near(t3, t4) && Near(T3, T4))) &&
        context.swap_stack.size() < param.max_swaps && Excludable(t3, t4)) {
      T3 = t3;
      T4 = t4;
      BestG2 = G2;
    }
  }
  *Gain = 0;
  if (T4) {
    // Make the best 2-opt move
    Swap1(t1, t2, T3);
    Exclude(t1, t2);
    Exclude(T3, T4);
    *G0 = BestG2;
  }
  return T4;
}

/*
   Below is shown the use of the variable X4 to discriminate between
   the 2 cases considered by the algorithm.

   The notation

        ab-

   is used for a subtour that starts with the edge (ta,tb). For example
   the tour

        12-43-

   contains the edges (t1,t2) and (t4,t3), in that order.

   X4 = 1:
       12-43-
   X4 = 2:
       12-34-
*/
