#include "data/Context.h"

/*
 * The TrimCandidateSet function takes care that each node has
 * associated at most MaxCandidates candidate edges.
 */

void TrimCandidateSet(int MaxCandidates) {
  Node *From = context.FirstNode;
  do {
    From->candidates.trim(MaxCandidates);
  } while ((From = From->SucNode()) != context.FirstNode);
}
