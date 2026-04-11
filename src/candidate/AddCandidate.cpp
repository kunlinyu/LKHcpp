#include "candidate/CandidateFuncs.h"

/*
 * The AddCandidate function adds a given edge (From, To) to the set
 * of candidate edges associated with the node From. The cost and
 * alpha-value of the edge are passed as parameters to the function.
 *
 * The function has no effect if the edge is already in the candidate
 * set.
 *
 * If the edge was added, the function returns 1; otherwise 0.
 *
 * The function is called from the functions CreateDelaunaySet and
 * OrderCandidateSet.
 */

int AddCandidate(Node* From, Node* To, WeightType Cost, int Alpha) {
  if (From == To) return 0;
  From->candidates.push_back(To, Cost, Alpha);
  return 1;
}
