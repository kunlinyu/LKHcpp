#pragma once
#include <algorithm>
#include <climits>
#include <functional>
#include <iostream>
#include <vector>

#include "data/Candidate.h"

using LessType = std::function<bool(const Candidate&, const Candidate&)>;

class CandidateSet {
 private:
  std::vector<Candidate> data_;

 public:
  void push_back(Node* to, WeightType cost, WeightType alpha = INT_MAX) {
    if (to == nullptr) return;
    for (auto& c : data_)
      if (c.To == to) {
        if (c.Alpha == INT_MAX) c = Candidate(to, cost, alpha);
        return;
      }
    data_.emplace_back(to, cost, alpha);
  }

  void insert(const Candidate& c, const LessType& less) {
    data_.emplace_back(c);
    size_t pos;
    for (pos = data_.size() - 1; pos > 0 && less(c, data_[pos - 1]); --pos)
      data_[pos] = data_[pos - 1];
    data_[pos] = c;
  }

  void erase(const std::function<bool(const Candidate&)>& bad) {
    data_.erase(std::remove_if(data_.begin(), data_.end(), bad), data_.end());
  }

  void trim(size_t max_candidates) {
    while (data_.size() > max_candidates) data_.pop_back();
  }

  void trim() {
    erase([](const Candidate& c) { return c.Alpha == INT_MAX; });
  }

  void sort(const LessType& less) {
    std::stable_sort(data_.begin(), data_.end(), less);
  }

  void reorder(const std::function<bool(const Candidate&)>& good) {
    for (size_t i = 0; i < data_.size(); ++i) {
      if (!good(data_[i])) continue;
      Candidate tmp = data_[i];
      for (size_t p = i; p > 0; --p) data_[p] = data_[p - 1];
      data_[0] = tmp;
    }
  }

  void clear() { data_.clear(); }

  size_t size() const { return data_.size(); }

  bool empty() const { return data_.empty(); }

  Candidate* find(Node* to) {
    for (auto& c : data_)
      if (c.To == to) return &c;
    return nullptr;
  }

  class iterator {
    using inner_iter = std::vector<Candidate>::iterator;
    inner_iter it_;

   public:
    explicit iterator(inner_iter it) : it_(it) {}
    iterator& operator++() {
      ++it_;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      ++it_;
      return tmp;
    }
    bool operator==(const iterator& other) const { return it_ == other.it_; }
    bool operator!=(const iterator& other) const { return it_ != other.it_; }
    Candidate* operator*() const { return &(*it_); }
    Candidate* operator->() const { return &(*it_); }
  };

  iterator begin() { return iterator(data_.begin()); }
  iterator end() { return iterator(data_.end()); }
};