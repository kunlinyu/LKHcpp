// @file STTSPVariant.h
// @brief
// @author Kunlin Yu
// @date 2026/4/9

#pragma once
#include "EdgeDataEncoder.h"
#include "data/Node.h"

class STTSP2TSP : public EdgeDataEncoder {
private:
  std::vector<Node> node_set;
 public:
  std::string chain() const override { return "STTSP2TSP -> " + VariantBase::chain(); }
  Problem Encode(const TSPLIB& tsplib) override {
    return EdgeDataEncoder::Encode(tsplib);
  }
  Tour Decode(const Tour& tour) override {
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