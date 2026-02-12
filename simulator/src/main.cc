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

  // -----------------------------------------------------------------
  // 1. Register available replacement policies
  // -----------------------------------------------------------------
  PolicyFactory::instance().register_policy(
      "LRU",
      [](size_t sets, size_t ways) -> std::unique_ptr<ReplacementPolicy> {
        return std::make_unique<LRU>(sets, ways);
      });

  // -----------------------------------------------------------------
  // 2. Configure the cache (hardcoded for now – later from JSON)
  // -----------------------------------------------------------------
  Cache::Config config;
  config.size_bytes = 32 * 1024; // 32 KB
  config.line_size_bytes = 64;   // 64 bytes
  config.associativity = 8;      // 8-way set associative
  config.replacement_policy = "LRU";

  Cache cache(config);

  // -----------------------------------------------------------------
  // 3. Open the trace file
  // -----------------------------------------------------------------
  auto reader = TraceReader::create_binary_reader();
  if (!reader->open(argv[1])) {
    return 1;
  }

  // -----------------------------------------------------------------
  // 4. Simulation loop
  // -----------------------------------------------------------------
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

  // -----------------------------------------------------------------
  // 5. Print results
  // -----------------------------------------------------------------
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
