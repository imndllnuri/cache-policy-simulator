#include "../inc/cache_set.h"
#include <cstdint>

CacheSet::CacheSet(size_t associativity,
                   std::unique_ptr<ReplacementPolicy> replacement_policy_)
    : cache_lines_(associativity), replacement_policy_(std::move(policy)) {}

bool CacheSet::access(uint64_t tag) {
  // Search for a hit
  for (size_t i = 0; i < cache_lines_.size(); ++i) {
    // is it hit?
    if (cache_lines_[i].valid && cache_lines_[i].tag == tag) {
      // TODO: access(i)
      (*replacement_policy_).access(i);
      return true;
    }
  }

  // if we are here we miss (damn fuck) TODO: get_victim
  size_t victim = (*replacement_policy_).get_victim();
  cache_lines_[victim].tag = tag;
  cache_lines_[victim].valid = true;

  (*replacement_policy_).insert(victim);

  return false;
}
