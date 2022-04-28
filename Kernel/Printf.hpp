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
int __attribute__((format(printf, 1, 2))) Printf(const char* fmtstr, ...);

#endif // PRINTF_H_