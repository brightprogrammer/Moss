/**
 * @file String.cpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 04/28/22
 * @modified 04/28/22
 * @brief Provide basic string utilities to kernel
 * @copyright MIT License 2022 Siddharth Mishra
 * */

#include "String.hpp"
#include <cstdarg>

static char int_to_string_buffer[128] = {0};

// get length of string
size_t strlen(const char* str){
    size_t sz = 0;
    while(*(str + sz)){
        sz++;
    }

    return sz;
}

// convert u64 to string
const char* itostr(int64_t n){
    // get size of string
    u8 digits = 1;

    u8 is_negative = 0;
    if(n < 0){
        is_negative = 1;
    }

    int_to_string_buffer[0] = '-';

    int64_t x = n;
    if(is_negative) x *= -1;
    while ((x /= 10) > 0)
        digits++;

    // convert to string
    x = n;
    if(is_negative) x *= -1;
    for(int8_t index = digits - 1; index >= 0; index--){
        int_to_string_buffer[index + is_negative] = (x % 10) + '0';
        x /= 10;
    }

    // null terminate string
    int_to_string_buffer[digits + is_negative] = '\0';

    return int_to_string_buffer;
}

// convert u64 to string
const char* utostr(u64 n){
    // get size of string
    u8 digits = 1;
    u64 x = n;
    while ((x /= 10) > 0)
        digits++;

    // convert to string
    x = n;
    for(int8_t index = digits - 1; index >= 0; index--){
        int_to_string_buffer[index] = (x % 10) + '0';
        x /= 10;
    }

    // null terminate string
    int_to_string_buffer[digits] = '\0';

    return int_to_string_buffer;
}

// convert u64 to hex string
const char* utohexstr(u64 n){
    // calculate size of hex string
    u8 size = 0;
    u64 x = n;
    while(x){
        x >>= 4;
        size++;
    }

    // hex digits in characters
    static const char* hex_digits = "0123456789abcdef";

    x = n;
    for(int8_t i = size - 1; i >= 0; i--){
        // null terminate string in the end
        if(x == 0){
            int_to_string_buffer[size] = 0;
            break;
        }

        // get lowest nibble and convert it to corresponding hex value
        u8 nibble = (u8)x & 0x0f;
        int_to_string_buffer[i] = hex_digits[nibble];
        x = x >> 4;
    }

    int_to_string_buffer[size] = 0;

    return int_to_string_buffer;
}

// compare two memory regions for n bytes
int64_t memcmp(const void* m1, const void* m2, size_t n){
    // Algorithm :
    // Try to compare 8 bytes at once if possible.
    // If 8 bytes at once is not possible then try 4 bytes at once.
    // if 4 bytes is not then 2 bytes and if 2 bytes is not possible then 1 byte.
    //
    // If n is less than 8 then check byte by byte as it makes code less complex.

    if(n > 8){
        // check remainder (remainder after dividing by 8) bytes
        const u8* u8m1 = reinterpret_cast<const u8*>(m1);
        const u8* u8m2 = reinterpret_cast<const u8*>(m2);
        size_t i = 0;
        for(i = 0; i < n % 8; i++){
            if(u8m1[i] != u8m2[i]) return (u8m1[i] - u8m2[i]);
        }

        // comparing 8 bytes at once will be faster since 64 bit register will
        // be used at once, this means less looping
        const u64* u64m1 = reinterpret_cast<const u64*>(u8m1 + i);
        const u64* u64m2 = reinterpret_cast<const u64*>(u8m2 + i);
        for(i = 0; i < n / 8 ; i++){
            if(u64m1[i] != u64m2[i]) return (u64m1[i] - u64m2[i]);
        }
    }else{
        // check remainder bytes
        const u8* u8m1 = reinterpret_cast<const u8*>(m1);
        const u8* u8m2 = reinterpret_cast<const u8*>(m2);
        size_t i = 0;
        for(i = 0; i < n; i++){
            if(u8m1[i] != u8m2[i]) return (u8m1[i] - u8m2[i]);
        }
    }

    return 0;
}

// copies the given byte to a bigger memory space by repeating it again and again
template<typename t>
t repeat_expand(char c){
    if(sizeof(t) == 64){
        return (c | c << 8 | c << 16 | c << 24 | u64(c) << 32 | u64(c) << 48 |  u64(c) << 56);
    } else if (sizeof(t) == 32){
        return (c | c << 8 | c << 16 | c << 24);
    } else if (sizeof(t) == 16){
        return (c | c << 8);
    } else {
        return c;
    }
}

// memset
void* memset(void* dst, u8 c, size_t n){
    // Algorithm
    // Try to write 8 bytes at once or 4 bytes or 2 bytes if possible.
    // If not possible then either write byte by byte for whole memory
    // or only for remainder number of bytes after dividing number of bytes by 8.

    if(n > 8){
        // set remainder bytes in the beginning to given byte
        u8* u8dst = reinterpret_cast<u8*>(dst);
        size_t i = 0;
        for(i = 0; i < n % 8; i++){
            u8dst[i] = c;
        }

        // memsetting 8 bytes at once will be faster
        u64* u64dst = reinterpret_cast<u64*>(u8dst + i);
        u64 C = repeat_expand<u64>(c);
        for(i = 0; i < n / 8 ; i++){
            u64dst[i] = C;
        }
    }else{
        // if less than 8 then just use byte by byte method
        u8* u8dst = reinterpret_cast<u8*>(dst);
        for(size_t i = 0; i < n; i++){
            u8dst[i] = c;
        }
    }

    return dst;
}

// copy memory from src to dst
void* memcpy(void *dst, const void* src, size_t n){
    // This follows similar approach as memset and memcmp

    if(n >= 8){
        // set remainder bytes in the beginning to given byte
        const u8* u8src = reinterpret_cast<const u8*>(src);
        u8* u8dst = reinterpret_cast<u8*>(dst);
        size_t i = 0;
        for(i = 0; i < n % 8; i++){
            u8dst[i] = u8src[i];
        }

        // memsetting 8 bytes at once will be faster
        const u64* u64src = reinterpret_cast<const u64*>(u8src + i);
        u64* u64dst = reinterpret_cast<u64*>(u8dst + i);
        for(i = 0; i < n / 8 ; i++){
            u64dst[i] = u64src[i];
        }
    }else{
        // if less than 8 then just use byte by byte method
        const u8* u8src = reinterpret_cast<const u8*>(src);
        u8* u8dst = reinterpret_cast<u8*>(dst);
        for(size_t i = 0; i < n; i++){
            u8dst[i] = u8src[i];
        }
    }

    return dst;
}

// check whether two strings are same or not
int64_t strcmp(const char* s1, const char* s2){
    if(strlen(s1) != strlen(s2)){
        return -1;
    }else{
        return memcmp(s1, s2, strlen(s1));
    }
}

// copy contents fromm src string to dst string
char* strcpy(char* dst, const char *src){
    return reinterpret_cast<char*>(memcpy(dst, src, strlen(src)));
}

// concatenate two strings
char* strcat(char* dst, const char* src){
    memcpy(dst + strlen(dst), src, strlen(src));
    dst[strlen(dst) + strlen(src) - 1] = 0;
    return dst;
}

// check if number (digit)
bool isdigit(char c){
    return (c >= '0') && (c <= '9');
}

// check if uppercase
bool isupper(char c){
    if(c >= 'A' && c <= 'Z'){
        return true;
    }

    return false;
}

// check if lowercase
bool islower(char c){
    if(c >= 'a' && c <= 'z'){
        return true;
    }

    return false;
}

// convert to uppercase
char toupper(char c){
    if(!isalpha(c)) return c;

    if(isupper(c)) return c;
    else{
        return 'A' + (c - 'a');
    }
}

// convert to lowercase
char tolower(char c){
    if(!isalpha(c)) return c;

    if(islower(c)) return c;
    else{
        return 'a' + (c - 'A');
    }
}

// check if alphabet
bool isalpha(char c){
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}


// check if alphabet or number
bool isalphanum(char c){
    return isalpha(c) || isdigit(c);
}

u32 __attribute__((format(printf, 2, 3)))
sprintf(char* buff, const char* fmtstr, ...){
    va_list vl;
    int i = 0, finalstrsz = 0;
    va_start(vl, fmtstr);

    while(fmtstr && fmtstr[i]){
        // check if any format specifier is present
        if(fmtstr[i] == '%'){
            i++;

            // start checkinf format specifiers one by one
            switch(fmtstr[i]){
                // print character
                case 'c': {
                    buff[finalstrsz] = static_cast<char>(va_arg(vl, int));
                    finalstrsz++;
                    break;
                }

                // print long values
                case 'l':{
                    i++;

                    switch(fmtstr[i]){
                        // print signed integer
                        case 'i':{
                            const char* tmp = nullptr;
                            tmp = itostr(va_arg(vl, int64_t));
                            strcpy(&buff[finalstrsz], tmp);
                            finalstrsz += strlen(tmp);
                            break;
                        }

                        // print unsigned integer
                        case 'u':{
                            const char* tmp = nullptr;
                            tmp = utostr(va_arg(vl, u64));
                            strcpy(&buff[finalstrsz], tmp);
                            finalstrsz += strlen(tmp);
                            break;
                        }

                        // print hex value
                        case 'x':{
                            const char* tmp = nullptr;
                            tmp = utohexstr(va_arg(vl, int64_t));
                            strcpy(&buff[finalstrsz], tmp);
                            finalstrsz += strlen(tmp);
                            break;
                        }

                        default: break;
                    }
                }

                // print integer
                case 'i':{
                    const char* tmp = nullptr;
                    tmp = itostr(va_arg(vl, int32_t));
                    strcpy(&buff[finalstrsz], tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }

                // print integer
                case 'u':{
                    const char* tmp = nullptr;
                    tmp = utostr(va_arg(vl, u32));
                    strcpy(&buff[finalstrsz], tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }

                case 'x':{
                    const char* tmp = nullptr;
                    tmp = utohexstr(va_arg(vl, u32));
                    strcpy(&buff[finalstrsz], tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }

                case 's':{
                    char* tmp = va_arg(vl, char*);
                    strcat(buff, tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }
            }
        }else{
            buff[finalstrsz] = fmtstr[i];
            finalstrsz++;
        }

        i++;
    }

    // null terminate final string
    buff[finalstrsz] = 0;

    va_end(vl);
    return finalstrsz;
}
