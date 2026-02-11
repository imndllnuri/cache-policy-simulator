#ifdef _WIN32
#include <pin.h>
#else
#include <pin.H>
#endif

#include "cache_trace.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

// Global file handles
FILE *outDebug = nullptr;
FILE *outLog = nullptr;
std::ofstream outTrace;
FILE *outTraceText = nullptr;

// PIN's KNOB system for command line options
KNOB<std::string> KnobOutTrace(KNOB_MODE_WRITEONCE, "pintool", "out",
                               "trace.bin", "trace output filename");
KNOB<BOOL> KnobDebug(KNOB_MODE_WRITEONCE, "pintool", "debug", "0",
                     "Enable debug logging");
KNOB<BOOL> KnobLog(KNOB_MODE_WRITEONCE, "pintool", "log", "0",
                   "Enable logging");
KNOB<BOOL> KnobTextTrace(KNOB_MODE_WRITEONCE, "pintool", "texttrace", "1",
                         "Enable text trace output (trace.txt)");

// Initialize debug output
static VOID InitDebug(BOOL enable_debug_print) {
  if (enable_debug_print && outDebug == nullptr) {
    outDebug = fopen("debug-out.txt", "w");
    if (outDebug == nullptr) {
      std::cerr << "[ERROR]: Could not open debug file" << std::endl;
    }
  }
}

// Initialize log output
static VOID InitLog(BOOL enable_log_print) {
  if (enable_log_print && outLog == nullptr) {
    outLog = fopen("log-out.txt", "w");
    if (outLog == nullptr) {
      std::cerr << "[ERROR]: Could not open log file" << std::endl;
    }
  }
}

// Initialize the output trace.bin file
VOID InitTrace(const char *out_path) {
  outTrace.open(out_path, std::ios::binary | std::ios::out);
  if (!outTrace.is_open()) {
    std::cerr << "[ERROR]: Could not open trace file: " << out_path
              << std::endl;
    PIN_ExitApplication(1);
  }

  if (KnobTextTrace.Value()) {
    outTraceText = fopen("trace.txt", "w");
    if (!outTraceText) {
      std::cerr << "[ERROR]: Could not open trace.txt" << std::endl;
      PIN_ExitApplication(1);
    }
  }
}

// Debug print helper function
VOID DebugPrint(const char *format, ...) {
  if (outDebug != nullptr) {
    va_list args;
    va_start(args, format);
    vfprintf(outDebug, format, args);
    fprintf(outDebug, "\n");
    fflush(outDebug);
    va_end(args);
  }
}

// Log print helper function
VOID LogPrint(const char *format, ...) {
  if (outLog != nullptr) {
    va_list args;
    va_start(args, format);
    vfprintf(outLog, format, args);
    fprintf(outLog, "\n");
    fflush(outLog);
    va_end(args);
  }
}

// Record memory access
VOID RecordMem(UINT64 instruction_pointer, UINT64 virtual_address,
               UINT8 instruction_size, BOOL is_write) {
  MemAccess record;
  record.virtual_address = virtual_address;
  record.instruction_pointer = instruction_pointer;
  record.instruction_size = instruction_size;
  record.isWrite = is_write;

  // Write the record to trace file
  outTrace.write(reinterpret_cast<const char *>(&record), sizeof(record));

  if (outTraceText) {
    fprintf(outTraceText, "%c IP=0x%lx VA=0x%lx SIZE=%u\n",
            is_write ? 'W' : 'R', (unsigned long)instruction_pointer,
            (unsigned long)virtual_address, (unsigned)instruction_size);
  }

  // Flush periodically to ensure data is written
  static int count = 0;
  if (++count % 1000 == 0) {
    outTrace.flush();
  }

  // Example debug output
  DebugPrint("[RECORDMEM]: IP=0x%lx, VA=0x%lx, size=%u, is_write=%d",
             instruction_pointer, virtual_address,
             (unsigned int)instruction_size, is_write);
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v) {
  // Instrument memory reads
  if (INS_IsMemoryRead(ins)) {
    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMem,
                             IARG_INST_PTR, IARG_MEMORYREAD_EA,
                             IARG_MEMORYREAD_SIZE, IARG_BOOL, FALSE, IARG_END);
    LogPrint("[INSTRUMENT]: Read at IP: 0x%lx", INS_Address(ins));
  }

  // Instrument memory writes (check if it has memory write)
  if (INS_IsMemoryWrite(ins)) {
    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMem,
                             IARG_INST_PTR, IARG_MEMORYWRITE_EA,
                             IARG_MEMORYWRITE_SIZE, IARG_BOOL, TRUE, IARG_END);
    LogPrint("[INSTRUMENT]: Write at IP: 0x%lx", INS_Address(ins));
  }
}

// Clean up trace resources
VOID FiniTrace() {
  if (outTrace.is_open()) {
    outTrace.flush();
    outTrace.close();
    DebugPrint("[FINITRACE]: Trace file closed");
  }
  if (outTraceText) {
    fclose(outTraceText);
    outTraceText = nullptr;
  }
  if (outDebug != nullptr) {
    fclose(outDebug);
    outDebug = nullptr;
  }
  if (outLog != nullptr) {
    fclose(outLog);
    outLog = nullptr;
  }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v) {
  FiniTrace();
  // Use a separate file for final message since logs might be closed
  FILE *finalMsg = fopen("pin_final.txt", "w");
  if (finalMsg) {
    fprintf(finalMsg, "[FINI]: Application exited with code: %d\n", code);
    fclose(finalMsg);
  }
}

INT32 Usage() {
  std::cerr << "===============================================" << std::endl;
  std::cerr << "This Pin tool records memory accesses for cache simulation"
            << std::endl;
  std::cerr << "===============================================" << std::endl;
  std::cerr << std::endl << KNOB_BASE::StringKnobSummary() << std::endl;
  return -1;
}

int main(int argc, char *argv[]) {
  // Initialize pin
  if (PIN_Init(argc, argv))
    return Usage();

  // Initialize outputs
  InitTrace(KnobOutTrace.Value().c_str());
  InitDebug(KnobDebug.Value());
  InitLog(KnobLog.Value());

  // Register Instruction to be called to instrument instructions
  INS_AddInstrumentFunction(Instruction, 0);

  // Register Fini to be called when the application exits
  PIN_AddFiniFunction(Fini, 0);

  DebugPrint("[MAIN]: Starting program instrumentation");

  // Never returns
  PIN_StartProgram();

  return 0;
}
