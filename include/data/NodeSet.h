// @file NodeSet.h
// @brief
// @author Kunlin Yu
// @date 2026/4/8
#pragma once

#include <vector>

#include "Node.h"
#include <plog/Log.h>

class NodeSet {
private:
  std::vector<Node> data_;

public:
  void CreateNodes(int Dimension) {
    PLOGF_IF(Dimension <= 0) << "DIMENSION is not positive (or not specified)";
    data_.resize(Dimension);
    for (int i = 0; i < Dimension; ++i) {
      auto *node = &data_[i];
      node->index = i;
      node->Id = node->OriginalId = i + 1;
      if (i > 0) Link(&data_[i - 1], node);
    }
    Link(&data_[Dimension - 1], &data_[0]);
  }
  Node *data(size_t index) { return &data_[index]; }
  Node &dataref(size_t index) { return data_[index]; }
  size_t size() const { return data_.size(); }
  void resize(size_t size) { data_.resize(size); }

  void ring_pair(const std::function<void(Node *, Node *)> &func) {
    size_t n = data_.size();
    if (n < 2) return;
    for (size_t i = 0; i < n; ++i) func(&data_[i], &data_[(i + 1) % n]);
  }

  std::vector<Node>::iterator begin() { return data_.begin(); }
  std::vector<Node>::iterator end() { return data_.end(); }
  std::vector<Node>::const_iterator begin() const { return data_.begin(); }
  std::vector<Node>::const_iterator end() const { return data_.end(); }
  std::vector<Node>::const_iterator cbegin() const { return data_.cbegin(); }
  std::vector<Node>::const_iterator cend() const { return data_.cend(); }
};