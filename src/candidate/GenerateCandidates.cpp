#include <plog/Log.h>

#include <climits>
#include <unordered_map>
#include <algorithm>

#include "candidate/CandidateFuncs.h"
#include "data/Candidate.h"
#include "data/Context.h"

/*
 * The GenerateCandidates function associates to each node a set of incident
 * candidate edges. The candidate edges of each node are sorted in increasing
 * order of their Alpha-values.
 *
 * The parameter MaxCandidates specifies the maximum number of candidate edges
 * allowed for each node, and MaxAlpha puts an upper limit on their
 * Alpha-values.
 *
 * A non-zero value of Symmetric specifies that the candidate set is to be
 * complemented such that every candidate edge is associated with both its
 * two end nodes (in this way MaxCandidates may be exceeded).
 *
 * The candidate edges of each node is kept in an array (CandidateSet) of
 * structures. Each structure (Candidate) holds the following information:
 *
 *      Node *To    : pointer to the other end node of the edge
 *      int Cost    : the cost (length) of the edge
 *      int Alpha   : the alpha-value of the edge
 *
 * The algorithm for computing Alpha-values in time O(n^2) and space O(n)
 * follows the description in
 *
 *      Keld Helsgaun,
 *      An Effective Implementation of the Lin-Kernighan Traveling
 *      Salesman Heuristic,
 *      Report, RUC, 1998.
 */

void GenerateCandidates(size_t MaxCandidates, GainType MaxAlpha, bool Symmetric) {
  LOGD << "Generating candidates ... ";
  if (MaxAlpha < 0 || MaxAlpha > INT_MAX) MaxAlpha = INT_MAX;
  // Initialize CandidateSet for each node
  for (auto &node : context.node_set) node.candidates.clear();
  Node *From = context.FirstNode;

  // Consider a path from "From" to "To" in the minimum spanning tree,
  // and let "Beta"(uppercase B) be the maximum cost of an edge on this path.
  // For node from "From" to common ancestor of "From" and "To",
  // beta[N] is the maximum cost of an edge on the path from "From" to "N".
  // For node from common ancestor of "From" and "To" to "To",
  // beta[N] is the maximum cost of an edge on the path from "From" to "N".
  // So the beta[To] is the final "Beta" for the path from "From" to "To".
  std::unordered_map<Node *, WeightType> beta;

  // mark[To] = From means that node "To" is on the path from "From" to root
  // of MST
  std::unordered_map<Node *, Node *> on_path_from;
  do on_path_from[From] = nullptr;
  while ((From = From->SucNode()) != context.FirstNode);

  if (MaxCandidates == 0) {
    do {
      if (From->candidates.empty()) {
        LOGE << "MAX_CANDIDATES = 0: No candidates";
        throw std::runtime_error("No candidates");
      }
    } while ((From = From->SucNode()) != context.FirstNode);
    return;
  }

  // Loop for each node "From"
  do {
    Node *To;
    if (From != context.FirstNode) {
      beta[From] = INT_MIN;
      for (To = From; To->Dad != nullptr; To = To->Dad) {
        beta[To->Dad] = std::max(beta[To], To->Cost);
        on_path_from[To->Dad] = From;
      }
    }
    To = context.FirstNode;
    // Loop for each node "To"
    do {
      if (To == From) continue;
      WeightType d = context.D(From, To);
      GainType a;
      if (From == context.FirstNode)
        a = To == From->Dad ? 0 : d - From->NextCost;
      else if (To == context.FirstNode)
        a = From == To->Dad ? 0 : d - To->NextCost;
      else {
        if (on_path_from[To] != From)
          beta[To] = std::max(beta[To->Dad], To->Cost);
        a = d - beta[To];
      }

      if (From == To) continue;
      if (a <= MaxAlpha && From != To) {
        // Insert new candidate edge in From->CandidateSet
        From->candidates.insert(Candidate(To, d, a), [](const Candidate &a,
                                                        const Candidate &b) {
          return a.Alpha < b.Alpha || (a.Alpha == b.Alpha && a.Cost < b.Cost);
        });
        From->candidates.trim(MaxCandidates);
      }
    } while ((To = To->SucNode()) != context.FirstNode);
  } while ((From = From->SucNode()) != context.FirstNode);

  if (Symmetric) SymmetrizeCandidateSet();
  LOGD << "done";
}
