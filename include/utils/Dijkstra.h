// @file Dijkstra.h
// @brief
// @author Kunlin Yu
// @date 2026-04-08

#pragma once
#include <climits>
#include <functional>
#include <map>
#include <set>
#include <vector>

#include "Heap.h"
#include "type.h"

template <typename NodeType>
struct SearchNode {
  NodeType parent;
  WeightType cost = INT_MAX / 2;
};

template <typename NodeType>
using NeighborFunction =
    std::function<std::vector<std::pair<NodeType, WeightType>>(NodeType)>;

template <typename NodeType>
using BreakFunction = std::function<bool(NodeType)>;

template <typename NodeType>
static std::map<NodeType, SearchNode<NodeType>> Dijkstra(
    NodeType source, const NeighborFunction<NodeType> &neighbors,
    const BreakFunction<NodeType> &break_condition) {
  std::map<NodeType, SearchNode<NodeType>> record;
  std::set<NodeType> visited;

  Heap<NodeType> heap([&record](NodeType a, NodeType b) {
    return record[a].cost < record[b].cost;
  });
  heap.Insert(source);
  record[source].cost = 0;

  while (heap.size() > 0) {
    NodeType current = heap.DeleteMin();
    if (break_condition(current)) break;
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