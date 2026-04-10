// @file Dijkstra.h
// @brief
// @author Kunlin Yu
// @date 2026-04-08

#pragma once
#include <climits>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Heap.h"
#include "type.h"

template <typename NodeType>
struct SearchNode {
  NodeType parent;
  WeightType cost = INT_MAX / 2;
};

template <typename NodeType>
using NeighborFunc =
    std::function<std::vector<std::pair<NodeType, WeightType>>(NodeType)>;

template <typename NodeType>
using Breaker = std::function<bool(NodeType)>;

template <typename NodeType,
          template <typename...> class MapType = std::unordered_map>
MapType<NodeType, SearchNode<NodeType>> Dijkstra(
    const NodeType &source, const NeighborFunc<NodeType> &neighbors,
    const Breaker<NodeType> &break_condition) {
  MapType<NodeType, SearchNode<NodeType>> record;
  std::unordered_set<NodeType> visited;

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