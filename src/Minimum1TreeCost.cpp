#include <cassert>
#include <map>

#include "Graph.h"
#include "LKH.h"
#include "data/Context.h"

/*
 * The Minimum1TreeCost function returns the cost of a minimum 1-tree.
 *
 * The minimum 1-tre is found by determining the minimum spanning tree and
 * then adding an edge corresponding to the second nearest neighbor of one
 * of the leaves of the tree (any node which has degree 1). The leaf chosen
 * is the one that has the longest second nearest neighbor distance.
 *
 * The V-value of a node is its degree minus 2. Therefore, Norm being the
 * sum of squares of all V-values, is a measure of a minimum 1-tree/s
 * discrepancy from a tour. If Norm is zero, then the 1-tree constitutes a
 * tour, and an optimal tour has been found.
 */

GainType Minimum1TreeCost(bool Sparse, std::map<Node *, int> &degree_2) {
  Node *N1 = nullptr;
  GainType Sum = 0;
  int Max = INT_MIN;

  MinimumSpanningTree(Sparse);
  Node *N = context.FirstNode;
  do {
    degree_2[N] = -2;
    Sum += N->Pi;
  } while ((N = N->SucNode()) != context.FirstNode);
  Sum *= -2;
  while ((N = N->SucNode()) != context.FirstNode) {
    degree_2[N]++;
    degree_2[N->Dad]++;
    Sum += N->Cost;
    N->Next = nullptr;
  }
  context.FirstNode->Dad = context.FirstNode->SucNode();
  context.FirstNode->Cost = context.FirstNode->SucNode()->Cost;
  do {
    if (degree_2[N] == -1) {
      Connect(N, Max, Sparse);
      if (N->NextCost > Max && N->Next) {
        N1 = N;
        Max = N->NextCost;
      }
    }
  } while ((N = N->SucNode()) != context.FirstNode);
  assert(N1);
  degree_2[N1->Next]++;
  degree_2[N1]++;
  Sum += N1->NextCost;
  context.Norm = 0;
  do context.Norm += degree_2[N] * degree_2[N];
  while ((N = N->SucNode()) != context.FirstNode);
  if (N1 == context.FirstNode)
    N1->SucNode()->Dad = nullptr;
  else {
    context.FirstNode->Dad = nullptr;
    Precede(N1, context.FirstNode);
    context.FirstNode = N1;
  }
  if (context.Norm == 0) {
    for (N = context.FirstNode->Dad; N; N1 = N, N = N->Dad) Follow(N, N1);
    for (N = context.FirstNode->SucNode(); N != context.FirstNode;
         N = N->SucNode()) {
      N->Dad = N->PrdNode();
      N->Cost = context.D(N, N->Dad);
    }
    context.FirstNode->SucNode()->Dad = nullptr;
  }
  return Sum;
}
