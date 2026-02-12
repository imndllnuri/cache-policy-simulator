#include "../inc/trace_reader.h"
#include "../inc/trace_format.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/types.h>

class BinaryTraceReader : public TraceReader {
public:
  BinaryTraceReader() = default;
  ~BinaryTraceReader() override { close(); };

  bool open(const std::string &file_path) override {
    file_.open(file_path, std::ios::binary);

    if (!file_.is_open()) {
      std::cerr << "[ERROR] Cannot open trace file" << std::endl;
      return false;
    }

    count_ = 0;
    return true;
  }

  bool next_access(MemAccess &access) override {

    if (!file_.is_open())
      return false;

    file_.read(reinterpret_cast<char *>(&access), sizeof(MemAccess));

    if (file_.gcount() != sizeof(MemAccess)) {
      if (file_.eof())
        return false;
      std::cerr << "[ERROR] Failed to read full MemAccess record" << std::endl;
      return false;
    }
    ++count_;
    return true;
  }

  void close() override {
    if (file_.is_open()) {
      file_.close();
    }
  }

  uint64_t access_count() const override { return count_; }

private:
  std::ifstream file_;
  uint64_t count_ = 0;
};

std::unique_ptr<TraceReader> TraceReader::create_binary_reader() {
  return std::make_unique<BinaryTraceReader>();
}
