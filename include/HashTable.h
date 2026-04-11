#pragma once

#include <set>
#include <vector>

#include "type.h"

unsigned Random();

/*
 * The functions insert and search is used to maintain a hash table of tours.
 *
 * A hash function maps tours to locations in a hash table. Each time
 * a tour improvement has been found, the hash table is consulted to
 * see whether the new tour happens to be local optimum found earlier.
 * If this is the case, fruitless checkout time is avoided.
 */

struct HashTable {
 private:
  std::set<std::pair<unsigned, GainType>> hash_cost;
  unsigned hash_ = 0;
  std::vector<unsigned> rand_;

 public:
  void init_rand(int size) {
      rand_.resize(size);
      for (int i = 0; i < size; i++) rand_[i] = Random();
  }

  void reset_hash() { hash_ = 0; }

  void clear() { hash_cost.clear(); }

  void update_hash(size_t id1, size_t id2) { hash_ ^= rand_[id1] * rand_[id2]; }

  void update_hash(size_t id1, size_t id2, size_t id3, size_t id4) {
    hash_ ^= (rand_[id1] * rand_[id2]) ^ (rand_[id3] * rand_[id4]) ^
             (rand_[id2] * rand_[id3]) ^ (rand_[id4] * rand_[id1]);
  }

  void insert(GainType Cost) {
    hash_cost.insert(std::make_pair(hash_, Cost));
  }

  bool search(GainType Cost) {
    return hash_cost.find(std::make_pair(hash_, Cost)) != hash_cost.end();
  }
};
