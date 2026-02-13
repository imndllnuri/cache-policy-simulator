#include "../inc/simulator.h"
#include <iostream>

Simulator::Simulator(const SimulatorConfig& simulation_config)
    : simulation_config_(simulation_config),
      cache_(simulation_config_.cache_config),
      reader_(TraceReader::create_binary_reader()) {}

bool Simulator::run() {
    if (!reader_->open(simulation_config_.trace_file_path)) {
        std::cerr << "[ERROR] Cannot open trace file: " << simulation_config_.trace_file_path << "\n";
        return false;
    }

    MemAccess acc;
    bool in_warmup = true;

    // Reset everything to a clean state
    total_accesses_ = 0;
    warmup_done_ = 0;
    sim_accesses_ = 0;
    cache_.reset_stats();

    while (reader_->next_access(acc)) {
        total_accesses_++;

        // 1. ALWAYS update cache state (no matter the phase)
        cache_.access(acc.virtual_address, acc.is_write != 0);

        // 2. Check if we just finished warmup
        if (in_warmup && total_accesses_ == simulation_config_.warmup_accesses) {
            // Reset performance counters – cache content stays intact
            cache_.reset_stats();
            in_warmup = false;
            warmup_done_ = simulation_config_.warmup_accesses;
        }

        // 3. Count measurement accesses
        if (!in_warmup) {
            sim_accesses_++;
        }

        // 4. Stop if we have executed enough simulation accesses
        if (simulation_config_.simulation_accesses > 0 && sim_accesses_ >= simulation_config_.simulation_accesses) {
            break;
        }
    }

    // If we never finished warmup (trace shorter than warmup)
    if (in_warmup) {
        warmup_done_ = total_accesses_;
        // No measurement phase → stats are zero (already reset)
    }

    reader_->close();
    return true;
}
