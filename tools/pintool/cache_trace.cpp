#include "pin.H"
#include <iostream>

VOID Fini(INT32, VOID *) { std::cerr << "[cache_trace] fini\n"; }

int main(int argc, char *argv[]) {
  if (PIN_Init(argc, argv)) {
    return 1;
  }

  PIN_AddFiniFunction(Fini, 0);
  PIN_StartProgram(); // never returns
  return 0;
}
