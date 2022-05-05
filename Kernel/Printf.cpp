/**
 * @file Printf.cpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 04/28/22
 * @modified 04/28/22
 * @brief Kernel Printf implementation.
 * @copyright MIT License 2022 Siddharth Mishra
 * */

#include <cstdarg>
#include <cstdint>

#include "String.hpp"
#include "Renderer.hpp"
#include "FontData.hpp"

// 2KB printf buffer
static char kprintf_buff[2048];

// cursor position information
u32 xpos = 0; // x position for next character
u32 ypos = 0; // y position for next character
u32 lastLineXPos = 0; // x position of cursor in last line
// this is useful when there is an early carriage return.

// printf for kernel code
u32 __attribute__((format(printf, 1, 2))) Printf(const char* fmtstr, ...){
    va_list vl;
    va_start(vl, fmtstr);
    u32 finalstrsz = sprintf(kprintf_buff, fmtstr, vl);
    va_end(vl);

    // draw string automatically adjusts xpos and ypos
    DrawString(kprintf_buff, xpos, ypos);

    return finalstrsz;
}


u32 __attribute__((format(printf, 3, 4))) ColorPrintf(u32 fgColor, u32 bgColor, const char* fmtstr, ...){
    va_list vl;
    va_start(vl, fmtstr);
    u32 finalstrsz = sprintf(kprintf_buff, fmtstr, vl);
    va_end(vl);

    // draw string automatically adjusts xpos and ypos
    DrawString(kprintf_buff, xpos, ypos, fgColor, bgColor);

    return finalstrsz;
}
