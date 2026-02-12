#pragma once

#include "../inc/cache_line.h"
#include "../replacement/replacement_policy.h"
#include <cstdint>
#include <memory>
#include <vector>

class CacheSet {
public:
  CacheSet(size_t associativity,
           std::unique_ptr<ReplacementPolicy> replacement_policy);
  bool access(uint64_t tag);

private:
  std::vector<CacheLine> cache_lines_;
  std::unique_ptr<ReplacementPolicy> replacement_policy_;
};
