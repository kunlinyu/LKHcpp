#pragma once
#include <stack>
#include <stdexcept>

#include "data/Node.h"

// The SwapRecord structure is used to record 2-opt moves (swaps)
class SwapRecord {
 private:
  // The 4 nodes involved in a 2-opt move
  Node* t1;
  Node* t2;
  Node* t3;
  Node* t4;

 public:
  SwapRecord() { t1 = t2 = t3 = t4 = nullptr; }
  SwapRecord(Node* t1, Node* t2, Node* t3, Node* t4)
      : t1(t1), t2(t2), t3(t3), t4(t4) {}
  ~SwapRecord() = default;
  Node* T1() { return t1; }
  Node* T2() { return t2; }
  Node* T3() { return t3; }
  Node* T4() { return t4; }
  Node* get(int i) {
    if (i == 1) return t1;
    if (i == 2) return t2;
    if (i == 3) return t3;
    return t4;
  }
  void ResetExcluded() {
    t1->ResetExcluded();
    t2->ResetExcluded();
    t3->ResetExcluded();
    t4->ResetExcluded();
  }
};

class SwapStack {
 private:
  std::stack<SwapRecord> swap_stack_;
  size_t check_point_;

 public:
  SwapStack() { check_point_ = 0; }
  ~SwapStack() {}

  void set_check_point() { check_point_ = swap_stack_.size(); }

  void clear() {
    while (!swap_stack_.empty()) swap_stack_.pop();
    check_point_ = 0;
  }

  void clear_check_point() { check_point_ = 0; }

  size_t size() const { return swap_stack_.size(); }

  bool empty() const { return swap_stack_.empty(); }

  void push(SwapRecord record) { swap_stack_.push(record); }

  void push(Node* t1, Node* t2, Node* t3, Node* t4) { push({t1, t2, t3, t4}); }

  void restore(const std::function<void(SwapRecord&)>& restore) {
    while (swap_stack_.size() > check_point_) {
      auto record = pop();
      restore(record);
    }
  }

  SwapRecord pop() {
    if (swap_stack_.empty()) throw std::out_of_range("Stack underflow");
    auto top = swap_stack_.top();
    swap_stack_.pop();
    return top;
  }
};
