# Cache Policy Simulator

A two-stage toolchain for memory access tracing (Intel SDE + Pin) and configurable cache simulation with pluggable replacement policies.

## Overview

This project provides two tightly integrated components for memory system research:

* **Cache Tracer** – an Intel Pin tool (bundled inside Intel SDE) that records every memory access (read/write) of a real application into a compact binary trace.
* **Cache Simulator** – a modular, high-performance cache simulator that reads these traces, models configurable caches, and collects detailed statistics.

The entire toolchain is easy to set up, extensible, and ideal for teaching or prototyping replacement policies.

## Features

* ✅ One-command installation – `./install.sh` downloads SDE, sets up Pin, and builds the tracer.
* ✅ Binary trace generation – records address, PC, size, and read/write flag.
* ✅ Configurable cache – size, line size, associativity, hit/miss latencies.
* ✅ Warmup & measurement phases – separate counts for warming the cache and collecting stats.
* ✅ Pluggable replacement policies – factory-based; LRU included.
* ✅ Per-set policy instances – each cache set owns its own policy object.
* ✅ Cycle-aware statistics – hit rate, average cycles, AMAT (derived).
* ✅ Python launcher – `run.py` for one-line tracing.

## Prerequisites

* Linux (x86_64) – also works on macOS with minor adjustments (Windows not tested; may work with Pin adjustments)
* Bash, tar, make, g++ (or clang)
* CMake 3.10+
* wget or curl
* Python 3.6+ (optional, for `run.py`)

## Setup

### `./tools/setup/install.sh`

This script:

* Downloads Intel SDE (includes a Pin kit) into `tools/sde-kit/`
* Sets `PIN_ROOT` automatically
* Builds the tracer `cache_trace.so` inside `tools/pintool/`

## Build the Simulator

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Generating Memory Traces

Use the Pin tool to record memory accesses of any program.

### Using the Python Wrapper (recommended)

```bash
./scripts/run.py /bin/ls -l
```

All arguments after `--` (or the first non-option) are passed to the target program.
The trace is saved as `trace.bin` by default; override with `--out mytrace.bin`.

### Direct Pin Command

```bash
pin -t tools/pintool/obj-intel64/cache_trace.so -out trace.bin -- /bin/ls -l
```

### Available Tracer Knobs (defined in `cache_trace.cpp`)

| Knob         | Default   | Description                     |
| ------------ | --------- | ------------------------------- |
| `-out`       | trace.bin | Binary trace output file        |
| `-texttrace` | 1         | Also write human-readable trace |
| `-debug`     | 0         | Verbose debug logging           |
| `-log`       | 0         | Instrumentation logging         |

## Running Cache Simulations

The simulator `cache_sim` reads a binary trace and applies optional warmup/measurement limits.

```bash
./cache_sim <trace.bin> [--warmup N] [--sim N]
```

* `--warmup N` – first N accesses warm the cache (stats not counted). Default: 0
* `--sim N` – number of accesses to simulate after warmup. Default: 0 (run to end)

### Examples

```bash
# Run whole trace as measurement
./cache_sim ../../traces/ls.bin

# Warmup 20K, measure 50K
./cache_sim ../../traces/ls.bin --warmup 20000 --sim 50000
```

Note: The trace path is relative to the working directory; when inside `build/`, use `../../traces/...`.
