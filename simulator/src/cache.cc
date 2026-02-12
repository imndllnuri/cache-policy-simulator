#include "../inc/cache.h"
#include "../inc/cache_set.h"
#include "../inc/policy_factory.h"
#include <cassert>
#include <iostream>

Cache::~Cache() = default;

Cache::Cache(const Config &config)
    : line_size_(config.line_size_bytes), 
      num_sets_(config.size_bytes / (config.line_size_bytes * config.associativity)),
      associativity_(config.associativity),
      sets_(), hits_(0), misses_(0), read_hits_(0), read_misses_(0),
      write_hits_(0), write_misses_(0) {
  assert(num_sets_ > 0 &&
         "Cache too small for given line size and associativity");

  for (size_t i = 0; i < num_sets_; ++i) {
    auto policy = PolicyFactory::instance().create(
        config.replacement_policy,
        num_sets_,     // total number of sets (policy might ignore)
        associativity_ // associativity (needed for LRU counters)
    );

    if (!policy) {
      std::cerr << "[ERROR] Unknown replacement policy: "
                << config.replacement_policy << std::endl;
      std::exit(1);
    }

    sets_.push_back(
        std::make_unique<CacheSet>(associativity_, std::move(policy)));
  }
}

bool Cache::access(uint64_t address, bool is_write) {
  uint64_t block_addr = address / line_size_;
  uint64_t set_index = block_addr % num_sets_;
  uint64_t tag = block_addr;

  bool hit = sets_[set_index]->access(tag);

  if (hit) {
    hits_++;
    if (is_write)
      write_hits_++;
    else
      read_hits_++;
  } else {
    misses_++;
    if (is_write)
      write_misses_++;
    else
      read_misses_++;
  }

  return hit;
}

void Cache::reset_stats() {
  hits_ = misses_ = 0;
  read_hits_ = read_misses_ = 0;
  write_hits_ = write_misses_ = 0;
}
