// @file NodeSet.h
// @brief
// @author Kunlin Yu
// @date 2026/4/8
#pragma once

#include <plog/Log.h>

#include <vector>

#include "Node.h"

class NodeSet {
 private:
  std::vector<Node> data_;

 public:
  void CreateNodes(int Dimension) {
    PLOGF_IF(Dimension <= 0) << "DIMENSION is not positive (or not specified)";
    data_.resize(Dimension);
    for (size_t index = 0; index < data_.size(); ++index) {
      data_[index].index = index;
      data_[index].Id = data_[index].OriginalId = index + 1;
    }
    ring_pair([](Node *a, Node *b) { Link(a, b); });
  }
  size_t size() const { return data_.size(); }
  void resize(size_t size) { data_.resize(size); }
  Node &front() { return data_.front(); }
  Node &back() { return data_.back(); }
  void erase(std::vector<Node>::const_iterator begin,
             std::vector<Node>::const_iterator end) {
    data_.erase(begin, end);
  }

  Node& operator[](size_t index) { return data_[index]; }
  const Node& operator[](size_t index) const { return data_[index]; }

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