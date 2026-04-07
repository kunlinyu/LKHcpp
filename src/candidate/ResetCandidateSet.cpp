#include "candidate/CandidateFuncs.h"
#include "data/Candidate.h"
#include "data/Context.h"

/*
 * Each time a trial has resulted in a shorter tour the candidate set is
 * adjusted (by AdjustCandidateSet). The ResetCandidates function resets
 * the candidate set. The original order is re-established (using, and
 * edges with Alpha == INT_MAX are excluded.
 *
 * The function is called from FindTour and OrderCandidates.
 */

void ResetCandidateSet() {
  Node* From = context.FirstNode;
  do {
    if (From->candidates.empty()) continue;
    // Reorder the candidate array of From
    From->candidates.sort([](const Candidate& a, const Candidate& b) {
      return a.Alpha < b.Alpha || (a.Alpha == b.Alpha && a.Cost < b.Cost);
    });
    // Remove included edges
    From->candidates.trim();
    // Remove impossible candidates
    From->candidates.erase(
        [&From](const Candidate& c) { return From == c.To; });
  } while ((From = From->SucNode()) != context.FirstNode);
}
