#include "../inc/cache.h"
#include "../inc/policy_factory.h"
#include "../inc/trace_reader.h"
#include "../replacement/lru.h"
#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <trace.bin>\n";
    return 1;
  }

  PolicyFactory::instance().register_policy(
      "LRU",
      [](size_t sets, size_t ways) -> std::unique_ptr<ReplacementPolicy> {
        return std::make_unique<LRU>(sets, ways);
      });

  Cache::Config config;
  config.hit_latency = 1;
  config.miss_penalty = 100;
  config.size_bytes = 32 * 1024; // 32 KB
  config.line_size_bytes = 64;   // 64 bytes
  config.associativity = 8;      // 8-way set associative
  config.replacement_policy = "LRU";

  Cache cache(config);

  auto reader = TraceReader::create_binary_reader();
  if (!reader->open(argv[1])) {
    return 1;
  }

  MemAccess acc;
  uint64_t total_accesses = 0;

  auto start = std::chrono::high_resolution_clock::now();

  while (reader->next_access(acc)) {
    cache.access(acc.virtual_address, acc.is_write != 0);
    total_accesses++;
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  reader->close();

  std::cout << "\n========================================\n";
  std::cout << "      CACHE SIMULATION RESULTS\n";
  std::cout << "========================================\n";
  std::cout << "Trace file         : " << argv[1] << "\n";
  std::cout << "Cache size         : " << config.size_bytes / 1024 << " KB\n";
  std::cout << "Line size          : " << config.line_size_bytes << " B\n";
  std::cout << "Associativity      : " << config.associativity << "-way\n";
  std::cout << "Replacement policy : " << config.replacement_policy << "\n";
  std::cout << "----------------------------------------\n";
  std::cout << "Total accesses     : " << total_accesses << "\n";
  std::cout << "Hits               : " << cache.hits() << "\n";
  std::cout << "Misses             : " << cache.misses() << "\n";
  std::cout << "Hit rate           : "
            << (100.0 * cache.hits() / total_accesses) << "%\n";
  std::cout << "Total cycles       : " << cache.total_cycles() << "\n";
  std::cout << "Avg cycles/access  : " << (double)cache.total_cycles() / total_accesses << "\n";
  std::cout << "AMAT (cycles)      : " << (double)(cache.hits() * config.hit_latency + cache.misses() * (config.hit_latency + config.miss_penalty)) 
                                          / total_accesses << "\n";
  std::cout << "----------------------------------------\n";
  std::cout << "Read hits          : " << cache.read_hits() << "\n";
  std::cout << "Read misses        : " << cache.read_misses() << "\n";
  std::cout << "Write hits         : " << cache.write_hits() << "\n";
  std::cout << "Write misses       : " << cache.write_misses() << "\n";
  std::cout << "----------------------------------------\n";
  std::cout << "Simulation time    : " << elapsed.count() << " seconds\n";
  std::cout << "========================================\n";

  return 0;
}
