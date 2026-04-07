#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

template <typename T>
class Heap {
 private:
  std::vector<T*> heap_;
  std::unordered_map<T*, size_t> loc_;
  std::function<bool(T*, T*)> less_than_;

 public:
  explicit Heap(const std::function<bool(T*, T*)>& less_than)
      : less_than_(less_than) {}

  void Reserve(size_t size) { heap_.reserve(size); }

  void Insert(T* n) {
    LazyInsert(n);
    SiftUp(n);
  }

  void LazyInsert(T* n) {
    loc_[n] = heap_.size();
    heap_.push_back(n);
  }

  void Clear() {
    loc_.clear();
    heap_.clear();
  }

  T* DeleteMin() {
    if (heap_.empty()) return nullptr;

    T* remove = heap_.front();

    loc_[heap_.back()] = 0;
    heap_.front() = heap_.back();
    heap_.pop_back();

    if (not heap_.empty())
      SiftDown(heap_.front());

    loc_.erase(remove);
    return remove;
  }

  void SiftUp(T* n) {
    size_t loc = loc_[n];
    size_t parent = (loc - 1) / 2;
    while (loc > 0 && less_than_(n, heap_[parent])) {
      heap_[loc] = heap_[parent];
      loc_[heap_[loc]] = loc;
      loc = parent;
      parent = (loc - 1) / 2;
    }
    heap_[loc] = n;
    loc_[n] = loc;
  }

  void SiftDown(T* n) {
    size_t loc = loc_[n];
    size_t child;
    while ((child = loc * 2 + 1) < heap_.size()) {
      if (child + 1 < heap_.size() &&
          less_than_(heap_[child + 1], heap_[child]))
        child++;
      if (not less_than_(heap_[child], n)) break;
      heap_[loc] = heap_[child];
      loc_[heap_[loc]] = loc;
      loc = child;
    }
    heap_[loc] = n;
    loc_[n] = loc;
  }

  bool contains(T* n) { return loc_.find(n) != loc_.end(); }
};
