#include "../inc/simulator.h"
#include "../inc/policy_factory.h"
#include "../replacement/lru.h"
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // ---------- Argument parsing ----------
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] 
                  << " <trace.bin> [--warmup N] [--sim N]\n";
        return 1;
    }

    std::string trace_file = argv[1];
    uint64_t warmup = 0;   // default: 20K accesses
    uint64_t sim = 0;      // default: 50K accesses

    for (int i = 2; i < argc; i += 2) {
        std::string flag = argv[i];
        if (flag == "--warmup" && i+1 < argc) {
            warmup = std::stoull(argv[i+1]);
        } else if (flag == "--sim" && i+1 < argc) {
            sim = std::stoull(argv[i+1]);
        } else {
            std::cerr << "Unknown flag: " << flag << "\n";
            return 1;
        }
    }

    // ---------- Register replacement policy ----------
    PolicyFactory::instance().register_policy(
        "LRU",
        [](size_t sets, size_t ways) -> std::unique_ptr<ReplacementPolicy> {
            return std::make_unique<LRU>(sets, ways);
        });

    // ---------- Build simulator configuration ----------
    Simulator::SimulatorConfig simulation_config;
    simulation_config.trace_file_path = trace_file;
    simulation_config.warmup_accesses = warmup;
    simulation_config.simulation_accesses = sim;

    // Cache parameters
    simulation_config.cache_config.hit_latency = 1;
    simulation_config.cache_config.miss_penalty = 100;
    simulation_config.cache_config.size_bytes = 32 * 1024;
    simulation_config.cache_config.line_size_bytes = 64;
    simulation_config.cache_config.associativity = 8;
    simulation_config.cache_config.replacement_policy = "LRU";

    // ---------- Run simulation ----------
    Simulator sim_obj(simulation_config);

    auto start = std::chrono::high_resolution_clock::now();
    if (!sim_obj.run()) {
        return 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // ---------- Print results ----------
    uint64_t total = sim_obj.total_accesses();
    uint64_t warmup_done = sim_obj.warmup_accesses();
    uint64_t sim_accesses = sim_obj.simulation_accesses();

    std::cout << "\n========================================\n";
    std::cout << "      CACHE SIMULATION RESULTS\n";
    std::cout << "========================================\n";
    std::cout << "Trace file         : " << trace_file << "\n";
    std::cout << "Cache size         : " << simulation_config.cache_config.size_bytes / 1024 << " KB\n";
    std::cout << "Line size          : " << simulation_config.cache_config.line_size_bytes << " B\n";
    std::cout << "Associativity      : " << simulation_config.cache_config.associativity << "-way\n";
    std::cout << "Replacement policy : " << simulation_config.cache_config.replacement_policy << "\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Total accesses read: " << total << "\n";
    std::cout << "Warmup accesses    : " << warmup_done << "\n";
    std::cout << "Simulation accesses: " << sim_accesses << "\n";
    std::cout << "Hits (sim phase)   : " << sim_obj.hits() << "\n";
    std::cout << "Misses (sim phase) : " << sim_obj.misses() << "\n";
    if (sim_accesses > 0) {
        std::cout << "Hit rate (sim phase): "
                  << (100.0 * sim_obj.hits() / sim_accesses) << "%\n";
        std::cout << "Avg cycles/access   : "
                  << (double)sim_obj.total_cycles() / sim_accesses << "\n";
    }
    // ... (rest of stats: read/write breakdown, AMAT, time)
    std::cout << "----------------------------------------\n";
    std::cout << "Simulation time    : " << elapsed.count() << " seconds\n";
    std::cout << "========================================\n";

    return 0;
}
