---

# Cache Policy Simulator

A two-stage toolchain to **trace memory accesses with Intel Pin (via SDE)** and **simulate cache replacement policies** (LRU, FIFO, etc.) using configurable cache parameters.

## Features

* Trace generation: R/W/IFetch, size, PC, address, TID
* Configurable cache: size, line size, ways, policy
* Pluggable replacement policies (LRU, FIFO; extend later)

## Project Layout

```
tools/pintool/     # Trace generator (Pin)
tools/setup/       # SDE/Pin setup scripts
simulator/         # Cache simulator
traces/            # Generated traces
scripts/           # Run helpers
```

## Setup

```bash
./tools/setup/install.sh
source tools/setup/setvars.sh
```

## Usage (high-level)

1. Run pintool to generate `traces/cache_footprint.trace`
2. Run simulator with `simulator/cache_configuration.json`

## Trace Format

```
<tid> <pc> <addr> <size> <R|W|I>
```

## Config

Edit:

```
simulator/cache_configuration.json
```

## Roadmap

* Add more policies (PLRU, Random)
* Multi-level cache
* Stats export (CSV/JSON)

---
