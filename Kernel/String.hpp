/**
 * @file String.hpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 04/28/22
 * @modified 04/28/22
 * @brief Provide basic string utilities to kernel
 * @copyright MIT License 2022 Siddharth Mishra
 * */

#ifndef STRING_H_
#define STRING_H_

#include <cstdint>
#include <cstddef>
#include "Common.hpp"

/**
 * @brief Get the size of a null terminated C String.
 *
 * @param str String to calculate length for.
 * @return length of string.
 * */
size_t strlen(const char* str);

/**
 * @brief Convert integer to null terminated string.
 *
 * @param n Integer to convert to string.
 * @return pointer to string.
 * */
const char* itostr(int64_t n);

/**
 * @brief Convert unsigned integer to string.
 *
 * @param n Integer to convert to string.
 * @return pointer to string.
 * */
const char* utostr(u64 n);

// convert u64 to hex string, no 0x prefix is given
/**
 * @brief Convert given u64 value to hexadecimal string.
 * This can take signed integer as input too because hex values are same.
 *
 * @param n Integer to get hex string for.
 * @return pointer to hex string.
 * */
const char* utohexstr(u64 n);

// NOTE : the memory checks are not always byte by byte
// sometimes memory checks use u64 values too!
// so don't assume the interface to be same as std C/C++ interface

/**
 * @brief Compare first n bytes of memory m1 and m2.
 *
 * @param m1 Pointer to memory m1.
 * @param m2 Pointer to memory m2.
 * @param n Number of bytes to compare.
 * @return If memories are equal then 0 is returned.
 * Other values are returned if they aren't equal.
 *  */
int64_t memcmp(const void* m1, const void* m2, size_t n);

/**
 * @brief Set first n bytes of src to given uint8_t value.
 *
 * @param src Pointer to memory to perform memset on.
 * @param v Value to fill into memory.
 * @param n Number of bytes to fill.
 * @param pointer to src.
 * */
void* memset(void *src, uint8_t v, size_t n);

/**
 * @brief Copy first n bytes of memory from src to dst.
 *
 * @param dst Pointer to destination memory.
 * @param src Pointer to source memory.
 * @param n Number of bytes to copy into memory.
 * @return pointer to dst.
 * */
void* memcpy(void *dst, const void *src, size_t n);

/**
 * @brief Compare and check whether two strings are same or not.
 *
 * @param s1 First C String.
 * @param s2 Second C String.
 * @param If strings are equal then 0 is returned.
 * Other values are returned if they aren't equal.
 * */
int64_t strcmp(const char* s1, const char* s2);

/**
 * @brief Copy contents from src string to dst string.
 * Source string must be a null terminated C String and
 * the destination string will also be a null terminated C String.
 *
 * @param dst Destination string.
 * @param src Source string.
 * @return pointer to new string.
 * */
char *strcpy(char *dst, const char *src);

/**
 * @brief Concatenate src null terminated string to dst's right side.
 * dst will be a null terminated string.
 *
 * @param dst Pointer to memory to perform concatenation on.
 * @param src Pointer to memory that needs to be concatenated.
 * @return pointer to new memory.
 * */
char *strcat(char *dst, const char *src);

/**
 * @brief Check if given char is digit or not.
 *
 * @param c Character to check.
 * @return True if character is digit, false otherwise.
 * */
bool isdigit(char c);

/**
 * @brief Check if given character is in upper case or not.
 *
 * @param c Character to check.
 * @return True if character is uppercase, false otherwise.
 * */
bool isupper(char c);

/**
 * @brief Check if given character is in lower case or not.
 *
 * @param c Character to check.
 * @return True if character is lowercase, false otherwise.
 * */
bool islower(char c);

/**
 * @brief Check if given char is an alphabet.
 *
 * @param c Character to check.
 * @return True if character is an alphabet, false otherwise.
 * */
bool isalpha(char c);

//
/**
 * @brief Check if given char is alphabet or number.
 *
 * @param c Character to check.
 * @return True if character is an alphabet or number, false otherwise.
 * */
bool isalphanum(char c);

/**
 * @brief Convert given character to it's upper case analog.
 *
 * @param c Character to convert.
 * @return Character value converted to uppercase.
 * */
char toupper(char c);

/**
 * @brief Convert given character to it's lower case analog.
 *
 * @param c Character to convert.
 * @return Character value converted to lowercase.
 * */
char tolower(char c);

/**
 * @brief String printf for kernel.
 *
 * @param buff Buffer to print string into.
 * @param fmtstr Format string.
 * @param arguments Other arguments as specified in fmtstr.
 * @return length of final string.
 * */
u32 __attribute__((format(printf, 2, 3)))
sprintf(char* buff, const char* fmtstr, ...);
#endif // STRING_H_
