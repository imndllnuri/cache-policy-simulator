#include "../inc/trace_reader.h"
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <trace.bin>\n";
    return 1;
  }

  auto reader = TraceReader::create_binary_reader();
  if (!reader->open(argv[1])) {
    return 1;
  }

  MemAccess acc;
  const int PRINT_LIMIT = 20;
  int printed = 0;

  while (reader->next_access(acc)) {
    if (printed < PRINT_LIMIT) {
      std::cout << (acc.is_write ? 'W' : 'R') << "  IP 0x" << std::hex
                << acc.instruction_pointer << "  VA 0x" << acc.virtual_address
                << "  " << std::dec << static_cast<int>(acc.instruction_size)
                << std::endl;
      printed++;
    }
  }

  std::cout << "\nTotal accesses read: " << reader->access_count() << std::endl;
  reader->close();
  return 0;
}
