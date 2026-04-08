#include <algorithm>
#include <cassert>

#include "data/Candidate.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"
#include "utils/Heap.h"
#include "utils/RingPair.h"

static std::unordered_map<Node *, Node *> Dijkstra(Node *Source);

void STTSP2TSP(std::vector<std::vector<int>> &Matrix,
               const std::set<NodeIdType> &required) {
  int NewDimension = 0;
  Node *N1 = context.FirstNode, *N2;

  std::unordered_map<int, int> new_index;
  do {
    if (required.count(N1->Id)) new_index[N1->Id] = NewDimension++;
  } while ((N1 = N1->SucNode()) != context.FirstNode);
  Matrix.resize(NewDimension);
  for (int i = 0; i < NewDimension; i++) Matrix[i].resize(NewDimension);
  do {
    if (required.count(N1->Id)) {
      auto parent = Dijkstra(N1);
      N1->Paths.resize(NewDimension + 1);
      int i = new_index[N1->Id];
      N2 = context.FirstNode;
      do {
        if (N2 != N1 && required.count(N2->Id)) {
          int j = new_index[N2->Id];
          Matrix[i][j] = N2->Cost;
          Node *N = N2;
          while ((N = parent[N]) != N1)
            N1->Paths[j + 1].push_back(N->OriginalId);
          std::reverse(N1->Paths[j + 1].begin(), N1->Paths[j + 1].end());
        }
      } while ((N2 = N2->SucNode()) != context.FirstNode);
    }
  } while ((N1 = N1->SucNode()) != context.FirstNode);
  context.node_set.erase(
      std::remove_if(
          context.node_set.begin(), context.node_set.end(),
          [&required](const Node &node) { return !required.count(node.Id); }),
      context.node_set.end());
  for (auto &node : context.node_set) {
    NodeIdType old_id = node.Id;
    node.index = new_index[old_id];
    node.Id = new_index[old_id] + 1;
    node.candidates.clear();
  }

  context.node_set.resize(NewDimension);
  context.FirstNode = &context.node_set.front();
  RingPair<Node>(context.node_set, [](Node &a, Node &b) { Link(a, b); });
  problem.dimension = NewDimension;
}

static std::unordered_map<Node *, Node *> Dijkstra(Node *Source) {
  std::unordered_map<Node *, Node *> parent;
  std::unordered_map<Node *, GainType> cost;

  Node *N = Source;
  Node *Blue = Source;
  Blue->Cost = 0;

  Heap<Node*> heap([&cost](Node *a, Node *b) { return cost[a] < cost[b]; });
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
