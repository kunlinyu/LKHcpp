// @file STTSPVariant.h
// @brief
// @author Kunlin Yu
// @date 2026/4/9

#pragma once
#include "EdgeDataEncoder.h"

class STTSP2TSP : public EdgeDataEncoder {
 public:
  std::string chain() const override { return "STTSP2TSP -> " + VariantBase::chain(); }
  Problem Encode(const TSPLIB& tsplib) override {
    return EdgeDataEncoder::Encode(tsplib);
  }
};