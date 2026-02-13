#pragma once

#include "cache.h"
#include "trace_reader.h"
#include <cstdint>
#include <string>

class Simulator {
public:
  struct SimulatorConfig {
    Cache::Config cache_config;

    uint64_t warmup_accesses = 0;
    uint64_t simulation_accesses = 0; // if set to 0 run entire trace

    // optional: trace file path can be passed to run()
    std::string trace_file_path;
  };

  explicit Simulator(const SimulatorConfig& simlator_config);
  ~Simulator() = default;

  // Disable copy, enable move
  Simulator(const Simulator&) = delete;
  Simulator& operator=(const Simulator&) = delete;
  Simulator(Simulator&&) = default;
  Simulator& operator=(Simulator&&) = default;
  
  // run the full sim (warmup + simulation)
  bool run();

  // Access results after run()
  uint64_t total_accesses() const { return total_accesses_; }
  uint64_t warmup_accesses() const { return warmup_done_; }
  uint64_t simulation_accesses() const { return sim_accesses_; }

  // Forward cache stats (only measurement phase)
  uint64_t hits() const { return cache_.hits(); }
  uint64_t misses() const { return cache_.misses(); }
  uint64_t read_hits() const { return cache_.read_hits(); }
  uint64_t read_misses() const { return cache_.read_misses(); }
  uint64_t write_hits() const { return cache_.write_hits(); }
  uint64_t write_misses() const { return cache_.write_misses(); }
  uint64_t total_cycles() const { return cache_.total_cycles(); }

private:
    SimulatorConfig simulation_config_;
    Cache cache_;
    std::unique_ptr<TraceReader> reader_;

    // Counters
    uint64_t total_accesses_ = 0;   // total accesses processed
    uint64_t warmup_done_ = 0;      // actual warmup accesses executed
    uint64_t sim_accesses_ = 0;     // accesses during measurement phase
};
