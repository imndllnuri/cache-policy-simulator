#pragma once

#include <cstdint>

struct CacheLine {
  uint64_t tag = 0;
  bool valid = 0;
};
