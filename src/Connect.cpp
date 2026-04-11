#include <climits>

#include "data/Candidate.h"
#include "data/Context.h"

/*
 * Let T be a minimum spanning tree on the graph, and let N1 be a node of
 * degree one in T. The Connect function determines a shortest edge emanating
 * from N1, but not in T. At return, the Next field of N1 points to the end
 * node of the edge, and its NextCost field contains the cost of the edge.
 * However, the search for the shortest edge is stopped if an edge shorter
 * than a specified threshold (Max) is found.
 */

void Connect(Node *N1, WeightType Max, bool Sparse) {
  Node *N;
  Candidate *NN1;
  WeightType d;

  N1->Next = nullptr;
  N1->NextCost = INT_MAX;
  if (!Sparse || N1->candidates.size() < 2) {
    // Find the requested edge in a dense graph
    N = context.FirstNode;
    do {
      if (N == N1 || N == N1->Dad || N1 == N->Dad) continue;
      if (N1 != N && (d = context.D(N1, N)) < N1->NextCost) {
        N1->NextCost = d;
        if (d <= Max) return;
        N1->Next = N;
      }
    } while ((N = N->SucNode()) != context.FirstNode);
  } else {
    // Find the requested edge in a sparse graph
    for (const auto &c : N1->candidates) {
      NN1 = c;
      N = NN1->To;
      if (N == N1->Dad || N1 == N->Dad) continue;
      if (N1 != N && (d = NN1->Cost + N1->Pi + N->Pi) < N1->NextCost) {
        N1->NextCost = d;
        if (d <= Max) return;
        N1->Next = N;
      }
    }
  }
}
