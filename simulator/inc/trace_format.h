#ifndef TRACE_FORMAT_H
#define TRACE_FORMAT_H

#include <cstdint>

// Tracce Format fields respectively virtual address, instruction pointer,
// instruction size, is_write
struct MemAccess {
  uint64_t virtual_address;
  uint64_t instruction_pointer;
  uint8_t instruction_size;
  uint8_t is_write;
};

// AccessType enum class that has two fields READ and WRITE
// TODO: maybe we can add instruction fetch
enum class AccessType : uint8_t {
  READ = 0,
  WRITE = 1,
};

// Helper function to convert isWrite bit to AccessType enum
inline AccessType get_access_type(const MemAccess &access) {
  return access.is_write ? AccessType::WRITE : AccessType::READ;
}

#endif // trace_format_h
