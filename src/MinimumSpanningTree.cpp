#include <climits>

#include "data/Candidate.h"
#include "data/Context.h"
#include "utils/Heap.h"

/*
 * The MinimumSpanningTree function determines a minimum spanning tree using
 * Prim's algorithm.
 *
 * At return the Dad field of each node contains the father of the node, and
 * the Cost field contains cost of the corresponding edge. The nodes are
 * placed in a topological ordered list, i.e., for any node its father precedes
 * the node in the list. The fields Pred and Suc of a node are pointers to the
 * predecessor and successor node in this list.
 *
 * The function can be used to determine a minimum spanning tree in a dense
 * graph, or in a sparse graph (a graph determined by a candidate set).
 *
 * When the graph is sparse a priority queue, implemented as a binary heap,
 * is used  to speed up the determination of which edge to include next into
 * the tree. The Rank field of a node is used to contain its priority (usually
 * equal to the shortest distance (Cost) to nodes of the tree).
 */

void MinimumSpanningTree(bool Sparse) {
  Node *Blue;  // Points to the last node included in the tree
  Node *NextBlue =
      nullptr;  // Points to the provisional next node to be included
  Node *N;
  Candidate *NBlue;
  WeightType d;

  Blue = N = context.FirstNode;
  Blue->Dad = nullptr;  // The root of the tree has no father
  std::unordered_map<Node *, WeightType> rank;
  Heap<Node*> heap([&rank](Node *a, Node *b) { return rank[a] < rank[b]; });
  if (Sparse && not Blue->candidates.empty()) {
    // The graph is sparse
    // Insert all nodes in the heap
    while ((N = N->SucNode()) != context.FirstNode) {
      N->Dad = Blue;
      N->Cost = INT_MAX;
      rank[N] = INT_MAX;
      heap.LazyInsert(N);
    }
    // Update all neighbors to the blue node
    for (const auto &c : Blue->candidates) {
      NBlue = c;
      N = NBlue->To;
      N->Dad = Blue;
      N->Cost = NBlue->Cost + Blue->Pi + N->Pi;
      rank[N] = NBlue->Cost + Blue->Pi + N->Pi;
      heap.SiftUp(N);
    }
    // Loop as long as there are more nodes to include in the tree
    while ((NextBlue = heap.DeleteMin())) {
      Follow(NextBlue, Blue);
      Blue = NextBlue;
      // Update all neighbors to the blue node
      for (const auto &c : Blue->candidates) {
        NBlue = c;
        N = NBlue->To;
        if (not heap.contains(N)) continue;
        if ((d = NBlue->Cost + Blue->Pi + N->Pi) < N->Cost) {
          N->Dad = Blue;
          N->Cost = d;
          rank[N] = d;
          heap.SiftUp(N);
        }
      }
    }
  } else {
    // The graph is dense
    while ((N = N->SucNode()) != context.FirstNode) N->Cost = INT_MAX;
    // Loop as long as there a more nodes to include in the tree
    while ((N = Blue->SucNode()) != context.FirstNode) {
      WeightType Min = INT_MAX;
      // Update all non-blue nodes (the successors of Blue in the list)
      do {
        if (Blue != N && (d = context.D(Blue, N)) < N->Cost) {
          N->Cost = d;
          N->Dad = Blue;
        }
        if (N->Cost < Min) {
          Min = N->Cost;
          NextBlue = N;
        }
      } while ((N = N->SucNode()) != context.FirstNode);
      Follow(NextBlue, Blue);
      Blue = NextBlue;
    }
  }
}
