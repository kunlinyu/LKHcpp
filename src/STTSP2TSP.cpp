#include <algorithm>
#include <cassert>
#include <cstdlib>

#include "data/Candidate.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"
#include "utils/Heap.h"

static std::unordered_map<Node *, Node *> Dijkstra(Node *Source);

void STTSP2TSP(std::vector<std::vector<int>> &Matrix,
               const std::set<NodeIdType> &required) {
  int NewDimension = 0;
  Node *N1 = context.FirstNode, *N2;

  std::unordered_map<Node *, int> new_id;
  do {
    if (required.count(N1->Id)) new_id[N1] = NewDimension++;
  } while ((N1 = N1->SucNode()) != context.FirstNode);
  Matrix.resize(NewDimension);
  for (int i = 0; i < NewDimension; i++) Matrix[i].resize(NewDimension);
  do {
    if (required.count(N1->Id)) {
      auto parent = Dijkstra(N1);
      N1->Paths.resize(NewDimension + 1);
      int i = new_id[N1];
      N2 = context.FirstNode;
      do {
        if (N2 != N1 && required.count(N2->Id)) {
          int j = new_id[N2];
          Matrix[i][j] = N2->Cost;
          Node *N = N2;
          while ((N = parent[N]) != N1)
            N1->Paths[j + 1].push_back(N->OriginalId);
          std::reverse(N1->Paths[j + 1].begin(), N1->Paths[j + 1].end());
        }
      } while ((N2 = N2->SucNode()) != context.FirstNode);
    }
  } while ((N1 = N1->SucNode()) != context.FirstNode);
  for (int i = 0, j = 0; i < context.node_set.size(); i++) {
    N1 = context.node_set.data(i);
    if (required.count(N1->Id)) {
      N1->index = new_id[N1];
      N1->Id = new_id[N1] + 1;
      N1->C = Matrix[new_id[N1]].data();
      N1->candidates.clear();
      context.node_set.data(j)->candidates.clear();
      context.node_set.dataref(j) = *N1;
      j++;
    }
  }
  for (int i = 0; i < NewDimension; i++, N1 = N2) {
    N2 = context.node_set.data(i);
    if (i == 0)
      context.FirstNode = N2;
    else
      Link(N1, N2);
  }
  Link(N1, context.FirstNode);
  problem.dimension = NewDimension;
}

static std::unordered_map<Node *, Node *> Dijkstra(Node *Source) {
  std::unordered_map<Node *, Node *> parent;
  std::unordered_map<Node *, GainType> cost;

  Node *N = Source;
  Node *Blue = Source;
  Blue->Cost = 0;

  Heap<Node> heap([&cost](Node *a, Node *b) { return cost[a] < cost[b]; });
  heap.Reserve(problem.dimension);
  heap.Clear();
  while ((N = N->SucNode()) != Source) {
    parent[N] = Blue;
    N->Cost = INT_MAX / 2;
    cost[N] = INT_MAX / 2;
    heap.LazyInsert(N);
  }
  for (const auto &NBlue : Blue->candidates) {
    N = NBlue->To;
    parent[N] = Blue;
    N->Cost = NBlue->Cost;
    cost[N] = NBlue->Cost;
    heap.SiftUp(N);
  }
  int d;
  while ((Blue = heap.DeleteMin())) {
    for (const auto &NBlue : Blue->candidates) {
      N = NBlue->To;
      if (heap.contains(N) && (d = Blue->Cost + NBlue->Cost) < N->Cost) {
        parent[N] = Blue;
        N->Cost = d;
        cost[N] = d;
        heap.SiftUp(N);
      }
    }
  }
  return parent;
}
