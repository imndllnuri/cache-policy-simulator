#pragma once
#include <cstddef>
#include <cstdint>

// Use PIN types if available, otherwise use standard types
#ifndef PIN_H
typedef unsigned long long UINT64;
typedef unsigned char UINT8;
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

struct MemAccess {
  UINT64 virtual_address;
  UINT64 instruction_pointer;
  UINT8 instruction_size;
  BOOL isWrite;
};

// Function declarations
void InitTrace(const char *out_path);
void RecordMem(UINT64 instruction_pointer, UINT64 virtual_address,
               UINT8 instruction_size, BOOL is_write);
void FiniTrace();

// Debug/Log helper declarations
void DebugPrint(const char *format, ...);
void LogPrint(const char *format, ...);
