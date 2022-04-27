#ifndef STRING_H_
#define STRING_H_

#include <cstdint>
#include <cstddef>

/**
 * @brief Calculates size (length) of a null terminated C string.
 *
 * @param str String to calculate length for.
 * @return Size of string excluding null termination. */
size_t StringLength(const char* str);

#endif // STRING_H_
