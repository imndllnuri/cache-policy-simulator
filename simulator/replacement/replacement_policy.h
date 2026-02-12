#pragma once

#include <cstdint>
#include <string>
#include <sys/types.h>

class ReplacementPolicy {
public:
  virtual ~ReplacementPolicy() = default;

  // Called on a cache hit to update replacement state (e.g., move to MRU)
  virtual void access(uint64_t way) = 0;

  virtual void insert(uint64_t way) = 0;

  // get victim
  virtual uint64_t get_victim() = 0;

  virtual void reset() = 0;

  virtual std::string name() const = 0;
};
