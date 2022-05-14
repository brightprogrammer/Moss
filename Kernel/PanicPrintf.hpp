#ifndef PANIC_HPP
#define PANIC_HPP

#include "Common.hpp"

// NOTE that after calling this function, caller registers wont be set back to nromal state
// so panic must only be called in absolute panic state
void __attribute__((format(printf, 1, 2)))
__attribute__((no_caller_saved_registers))
PanicPrintf(const char* fmtstr, ...);

// panic puts
void __attribute__((no_caller_saved_registers))
PanicPuts(const char* str);

#endif // PANIC_HPP
