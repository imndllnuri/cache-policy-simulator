

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

class CacheSet;

class Cache {

  struct Config {
    uint64_t size_bytes;
    uint64_t line_size_bytes;
    uint64_t associativity;

    std::string ReplacementPolicy;
  };

  explicit Cache(const Config &config);

  // Returns true on hit, false on miss
  bool access(uint64_t address, bool is_write);

  // Statistics

  uint64_t hits() const { return hits_; }
  uint64_t miss() const { return miss_; }
  uint64_t read_hits() const { return read_hits_; }
  uint64_t read_miss() const { return read_miss_; }
  uint64_t write_hits() const { return write_hits_; }
  uint64_t write_miss() const { return write_miss_; }

  void reset_stats();

private:
  uint64_t line_size_;
  uint64_t num_sets_;
  uint64_t associativity_;
  std::vector<std::unique_ptr<CacheSet>> sets_;

  uint64_t hits_;
  uint64_t miss_;
  uint64_t read_hits_;
  uint64_t read_miss_;
  uint64_t write_hits_;
  uint64_t write_miss_;
};
