/**
 * @file Printf.hpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 04/28/22
 * @modified 04/28/22
 * @brief Kernel Printf declaration.
 * @copyright MIT License 2022 Siddharth Mishra
 * */

#ifndef PRINTF_H_
#define PRINTF_H_

#include "Common.hpp"
#include "Colors.hpp"

/**
 * @brief Kernel printf. The interface is a subset of that provided by C stdlib.
 * @warn This printf first creates a temporary string in a static buffer that
 * is then printed to screen. The size of this buffer is 2048 bytes. Hence if
 * the final string exceeds or reaches this limit then the behaviour is undefined.
 *
 * @param fmtstr Format string specifying how to print. Supported format specifiers
 * are %c, %i, %u, %x, %li, %lu, %lx and %s.
 * @return Number of bytes printed.
 * */
u32 __attribute__((format(printf, 1, 2))) Printf(const char* fmtstr, ...);

/**
 * @brief This internally calls the normal printf after resetting the color.
 *
 * @param fg Foreground color.
 * @param bg Background color.
 * @param fmtstr Format string.
 * @return Number of bytes printed */
u32 __attribute__((format(printf, 3, 4))) ColorPrintf(u32 fg, u32 bg, const char* fmtstr, ...);

#endif // PRINTF_H_
