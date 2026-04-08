#include <algorithm>
#include <cassert>

#include "data/Candidate.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"
#include "plog/Log.h"
#include "utils/Heap.h"
#include "utils/RingPair.h"

template <typename NodeType>
struct SearchNode {
  NodeType parent;
  WeightType cost = INT_MAX / 2;
};

template <typename NodeType>
static std::unordered_map<NodeType, SearchNode<NodeType>> Dijkstra(
    NodeType source,
    std::function<std::vector<std::pair<NodeType, WeightType>>(NodeType)>
        neighbors) {
  std::unordered_map<NodeType, SearchNode<NodeType>> record;
  std::set<NodeType> visited;

  Heap<NodeType> heap([&record](NodeType a, NodeType b) {
    return record[a].cost < record[b].cost;
  });
  heap.Insert(source);
  record[source].cost = 0;

  while (heap.size() > 0) {
    NodeType current = heap.DeleteMin();
    for (const auto &neighbor_cost : neighbors(current)) {
      NodeType neighbor = neighbor_cost.first;
      WeightType cost = neighbor_cost.second;
      if (visited.count(neighbor)) continue;

      int d = record[current].cost + cost;
      if (d < record[neighbor].cost) {
        record[neighbor].parent = current;
        record[neighbor].cost = d;
        if (not heap.contains(neighbor)) heap.LazyInsert(neighbor);
        heap.SiftUp(neighbor);
      }
    }
    visited.insert(current);
  }
  return record;
}

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
      auto record = Dijkstra<Node *>(N1, [](Node *node) {
        std::vector<std::pair<Node *, WeightType>> neighbors;
        for (const auto &c : node->candidates)
          neighbors.emplace_back(c->To, c->Cost);
        return neighbors;
      });
      if (record.size() < problem.dimension) {
        PLOGE << "the graph has multiple connected components";
        throw std::runtime_error("the graph has multiple connected components");
      }
      N1->Paths.resize(NewDimension + 1);
      int i = new_index[N1->Id];
      N2 = context.FirstNode;
      do {
        if (N2 != N1 && required.count(N2->Id)) {
          int j = new_index[N2->Id];
          Matrix[i][j] = record[N2].cost;
          Node *N = N2;
          while ((N = record[N].parent) != N1 and N != nullptr)
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