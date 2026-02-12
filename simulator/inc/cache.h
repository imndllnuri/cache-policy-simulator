#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class CacheSet;

class Cache {
public:
  struct Config {
    uint64_t size_bytes;
    uint64_t line_size_bytes;
    uint64_t associativity;
    std::string replacement_policy;
  };

  explicit Cache(const Config &config);
  ~Cache(); // <-- ADD THIS

  // Disable copy (prevents accidental copies with incomplete type)
  Cache(const Cache &) = delete;
  Cache &operator=(const Cache &) = delete;

  // Allow move (define in .cc if needed, but not required now)
  Cache(Cache &&) = default;
  Cache &operator=(Cache &&) = default;

  bool access(uint64_t address, bool is_write);

  uint64_t hits() const { return hits_; }
  uint64_t misses() const { return misses_; }
  uint64_t read_hits() const { return read_hits_; }
  uint64_t read_misses() const { return read_misses_; }
  uint64_t write_hits() const { return write_hits_; }
  uint64_t write_misses() const { return write_misses_; }

  void reset_stats();

private:
  uint64_t line_size_;
  uint64_t num_sets_;
  uint64_t associativity_;
  std::vector<std::unique_ptr<CacheSet>> sets_;

  uint64_t hits_ = 0;
  uint64_t misses_ = 0;
  uint64_t read_hits_ = 0;
  uint64_t read_misses_ = 0;
  uint64_t write_hits_ = 0;
  uint64_t write_misses_ = 0;
};
