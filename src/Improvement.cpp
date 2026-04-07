#include "Function.h"
#include "LKH.h"

/* The Improvement function is used to check whether a done move
 * has improved the current best tour.
 * If the tour has been improved, the function computes the penalty gain
 * and returns 1. Otherwise, the move is undone, and the function returns 0.
 */

int Improvement(GainType* Gain, Node* t1, Node* SUCt1) {
  if (*Gain > 0) return 1;
  RestoreTour();
  if (SUC(t1) != SUCt1) context.Reversed = !context.Reversed;
  *Gain = 0;
  return 0;
}