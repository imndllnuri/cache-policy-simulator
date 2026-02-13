cmake_minimum_required(VERSION 3.10)
project(CacheSimulator VERSION 1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# --- Source files ---
set(SIM_SOURCES
    src/cache.cc
    src/cache_set.cc
    src/policy_factory.cc
    src/trace_reader.cc
    src/main.cc
    src/simulator.cc
    replacement/lru.cc
)

# --- Executable ---
add_executable(cache_sim ${SIM_SOURCES})

# --- Include directories (after target creation) ---
target_include_directories(cache_sim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/replacement
)

# --- Compiler flags ---
target_compile_options(cache_sim PRIVATE -Wall -Wextra -g)

# --- Optional: generate compile_commands.json ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
