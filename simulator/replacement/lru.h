#pragma once

#include "replacement_policy.h"
#include <vector>

class LRU : public ReplacementPolicy {
public:
  LRU(size_t sets, size_t associativity);
  void access(uint64_t way) override;
  void insert(uint64_t way) override;
  uint64_t get_victim() override;
  void reset() override;
  std::string name() const override { return "LRU"; }

private:
  size_t associativity_;
  std::vector<uint64_t> lru_counters_; // per-way timestamps
  uint64_t timestamp_;
};
