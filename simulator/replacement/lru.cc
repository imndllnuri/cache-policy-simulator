#include "lru.h"
#include <algorithm>

LRU::LRU(size_t sets, size_t associativity)
    : associativity_(associativity), lru_counters_(associativity, 0),
      timestamp_(0) {
  // sets is ignored; LRU is per-set, but we'll create one policy per set
  (void)sets;
}

void LRU::access(uint64_t way) { lru_counters_[way] = ++timestamp_; }

void LRU::insert(uint64_t way) { lru_counters_[way] = ++timestamp_; }

uint64_t LRU::get_victim() {
  return std::min_element(lru_counters_.begin(), lru_counters_.end()) -
         lru_counters_.begin();
}

void LRU::reset() {
  timestamp_ = 0;
  std::fill(lru_counters_.begin(), lru_counters_.end(), 0);
}
