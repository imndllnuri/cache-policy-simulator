#pragma once

#include "trace_format.h"
#include <memory>
#include <string>

class TraceReader {
public:
  virtual ~TraceReader() = default;

  // Opens trace file. Returns true on success
  virtual bool open(const std::string &file_path) = 0;

  // Read the next access into "access". Returns false on EOF or error.
  virtual bool next_access(MemAccess &access) = 0;

  // Close the file
  virtual void close() = 0;

  // Total number of access successfully read so far
  virtual uint64_t access_count() const = 0;

  static std::unique_ptr<TraceReader> create_binary_reader();
};
