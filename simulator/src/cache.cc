
#include "../inc/cache.h"
#include <cassert>

Cache::Cache(const Config &config)
    : line_size_(config.line_size_bytes),
      num_sets_(config.size_bytes /
                (config.line_size_bytes * config.associativity)),
      sets_() {
  assert(num_sets_ > 0 &&
         "Cache too small for given line size and associativity");

  for (size_t i = 0; i < num_sets_; i++) {
  }
}
