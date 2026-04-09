#include <algorithm>
#include <map>
#include <vector>

#include "data/Candidate.h"
#include "data/Context.h"
#include "data/Problem.h"
#include "utils/Dijkstra.h"
#include "utils/Heap.h"
#include "utils/RingPair.h"



std::vector<std::vector<int>> STTSP2TSP(const std::set<NodeIdType> &required) {
  int NewDimension = 0;
  Node *N1 = context.FirstNode, *N2;

  std::unordered_map<int, int> new_index;
  do {
    if (required.count(N1->Id)) new_index[N1->Id] = NewDimension++;
  } while ((N1 = N1->SucNode()) != context.FirstNode);
  std::vector<std::vector<int>> Matrix;

  Matrix.resize(NewDimension);

  for (int i = 0; i < NewDimension; i++) Matrix[i].resize(NewDimension);
  do {
    if (required.count(N1->Id)) {
      std::set<NodeIdType> target = required;
      auto record = Dijkstra<Node *>(
          N1,
          [](Node *node) {
            std::vector<std::pair<Node *, WeightType>> neighbors;
            for (const auto &c : node->candidates)
              neighbors.emplace_back(c->To, c->Cost);
            return neighbors;
          },
          [target](Node *node) mutable {
            if (target.count(node->Id)) target.erase(node->Id);
            return target.empty();
          });
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

  return Matrix;
}