#include "data/Candidate.h"
#include "data/Context.h"

/*
 * When a trial has produced a better tour, the set of candidate edges is
 * adjusted as follows:
 *
 *   (1) The set is extended with tour edges not present in the
 *       current set;
 *   (2) Precedence is given to those edges that are common to the two
 *       currently best tours.
 *
 * The AdjustCandidateSet function adjusts for each node its table of
 * candidate edges. A new candidate edge is added by extending the table
 * and inserting the edge as its last ordinary element (disregarding the
 * dummy edge). The Alpha field of the new candidate edge is set to
 * INT_MAX. Edges that belong to the best tour as well as the next best
 * tour are moved to the start of the table.
 */

void AdjustCandidateSet(Node* FirstNode) {
  Node* node = FirstNode;
  do {
    // Extend
    node->candidates.push_back(node->PrdNode(),
                               context.C(node, node->PrdNode()));
    node->candidates.push_back(node->SucNode(),
                               context.C(node, node->SucNode()));
    // Reorder
    node->candidates.reorder([&](const Candidate& c) {
      return InBestTour(node, c.To) && InNextBestTour(node, c.To);
    });
  } while ((node = node->SucNode()) != FirstNode);
}
