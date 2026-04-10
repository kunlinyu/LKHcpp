// @file NodeCoordEncoder.h
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#pragma once
#include "VariantBase.h"

class NodeCoordEncoder : public VariantBase {
 public:
  std::string chain() const override {
    return "NodeCoordEncoder -> " + VariantBase::chain();
  }
  Problem Encode(const TSPLIB& tsplib) override;
};