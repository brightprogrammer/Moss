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


// draw a string without any formatting
void Puts(const char* str){
    DrawString(str, xpos, ypos);
}

// puts but with a color
void ColorPuts(uint32_t fgcolor, uint32_t bgcolor, const char* str){
    // draw with new colors
    DrawString(str, xpos, ypos, fgcolor, bgcolor);
}

// draw a string without any formatting
void PutChar(char c){
    if(!c){
        return;
    }

    DrawCharacter(c, xpos, ypos);
}

// puts but with a color
void ColorPutChar(uint32_t fgcolor, uint32_t bgcolor, char c){
    if(!c){
        return;
    }
    // draw with new colors
    DrawCharacter(c, xpos, ypos, fgcolor, bgcolor);
}
