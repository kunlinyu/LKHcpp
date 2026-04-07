#include "candidate/CandidateFuncs.h"
#include "data/Candidate.h"
#include "data/Context.h"

/*
 * The SymmetrizeCandidateSet function complements the candidate set such
 * that every candidate edge is associated with both its two end nodes.
 */

void SymmetrizeCandidateSet() {
  Node* From;

  From = context.FirstNode;
  do {
    for (const auto& NFrom : From->candidates) {
      AddCandidate(NFrom->To, From, NFrom->Cost, NFrom->Alpha);
    }
  } while ((From = From->SucNode()) != context.FirstNode);
  ResetCandidateSet();
}
