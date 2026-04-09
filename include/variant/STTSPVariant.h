// @file STTSPVariant.h
// @brief
// @author Kunlin Yu
// @date 2026/4/9

#pragma once
#include <unordered_map>

#include "EdgeDataEncoder.h"
#include "Initializer.h"
#include "data/Node.h"
#include "utils/Dijkstra.h"

class STTSPVariant : public VariantBase {
 private:
  std::vector<Node> node_set;

 public:
  std::string chain() const override {
    return "STTSP2TSP -> " + VariantBase::chain();
  }
  Problem Encode(const TSPLIB &tsplib) override {
    auto &required = tsplib.required_nodes_section;
    auto &edge_data_section = tsplib.edge_data_section;
    int NewDimension = 0;

    node_set = Initializer::CreateNodes(tsplib.dimension);
    Node *N1 = &node_set.front(), *N2;

    std::unordered_map<int, int> new_index;
    do {
      if (required.count(N1->Id)) new_index[N1->Id] = NewDimension++;
    } while ((N1 = N1->SucNode()) != &node_set.front());

    std::unordered_map<NodeIdType, std::unordered_map<NodeIdType, WeightType>>
        node_2_edges;
    for (const auto &edge_data : edge_data_section) {
      node_2_edges[edge_data.from].insert(
          std::make_pair(edge_data.to, edge_data.weight));
      node_2_edges[edge_data.to].insert(
          std::make_pair(edge_data.from, edge_data.weight));
    }

    std::vector<std::vector<WeightType>> Matrix;
    Matrix.resize(NewDimension);

    for (int i = 0; i < NewDimension; i++) Matrix[i].resize(NewDimension);
    do {
      if (required.count(N1->Id)) {
        std::set<NodeIdType> target = required;
        auto record = Dijkstra<Node *>(
            N1,
            [&node_2_edges, this](Node *node) {
              std::vector<std::pair<Node *, WeightType>> neighbors;
              for (const auto &pair : node_2_edges[node->Id])
                neighbors.emplace_back(&node_set[pair.first - 1], pair.second);
              return neighbors;
            },
            [target](Node *node) mutable {
              if (target.count(node->Id)) target.erase(node->Id);
              return target.empty();
            });
        N1->Paths.resize(NewDimension + 1);
        int i = new_index[N1->Id];
        N2 = &node_set.front();
        do {
          if (N2 != N1 && required.count(N2->Id)) {
            int j = new_index[N2->Id];
            Matrix[i][j] = record[N2].cost;
            Node *N = N2;
            while ((N = record[N].parent) != N1 and N != nullptr)
              N1->Paths[j + 1].push_back(N->OriginalId);
            std::reverse(N1->Paths[j + 1].begin(), N1->Paths[j + 1].end());
          }
        } while ((N2 = N2->SucNode()) != &node_set.front());
      }
    } while ((N1 = N1->SucNode()) != &node_set.front());
    node_set.erase(std::remove_if(node_set.begin(), node_set.end(),
                                  [&required](const Node &node) {
                                    return !required.count(node.Id);
                                  }),
                   node_set.end());
    for (auto &node : node_set) {
      NodeIdType old_id = node.Id;
      node.index = new_index[old_id];
      node.Id = new_index[old_id] + 1;
      node.candidates.clear();
    }

    RingPair<Node>(node_set, [](Node &a, Node &b) { Link(a, b); });

    return Problem(NewDimension, Matrix);
  }
  Tour Decode(const Tour &tour) override {
    Tour result;
    for (size_t i = 0; i < tour.node_ids.size(); i++) {
      size_t next_i = i + 1;
      if (next_i >= tour.node_ids.size()) next_i = 0;

      NodeIdType a = tour.node_ids[i];
      NodeIdType b = tour.node_ids[next_i];

      // TODO: convert id to index
      result.node_ids.push_back(node_set[a - 1].OriginalId);
      for (const int k : node_set[a - 1].Paths[b])
        result.node_ids.push_back(k);
    }
    return result;
  };
};