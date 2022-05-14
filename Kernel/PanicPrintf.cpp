#include <cstdint>
#include <cstdarg>
#include <cwctype>

#include "Printf.hpp"
#include "PanicPrintf.hpp"
#include "Renderer.hpp"
#include "String.hpp"


void __attribute__((format(printf, 1, 2)))
__attribute__((no_caller_saved_registers))
PanicPrintf(const char* fmtstr, ...){
    // print
    va_list vl;
    va_start(vl, fmtstr);
    ColorPrintf(COLOR_RED, COLOR_BLACK, fmtstr, vl);
    va_end(vl);
}

// normal print without formatting
void  __attribute__((no_caller_saved_registers))
PanicPuts(const char* str){
    // draw string
    ColorPuts(COLOR_RED, COLOR_BLACK, str);
}
