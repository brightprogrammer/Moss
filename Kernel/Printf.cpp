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
uint32_t xpos = 0; // x position for next character
uint32_t ypos = 0; // y position for next character
uint32_t lastLineXPos = 0; // x position of cursor in last line
// this is useful when there is an early carriage return.

// printf for kernel code
int __attribute__((format(printf, 1, 2))) Printf(const char* fmtstr, ...){
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
                    kprintf_buff[finalstrsz] = static_cast<char>(va_arg(vl, int));
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
                            strcpy(&kprintf_buff[finalstrsz], tmp);
                            finalstrsz += strlen(tmp);
                            break;
                        }

                        // print unsigned integer
                        case 'u':{
                            const char* tmp = nullptr;
                            tmp = utostr(va_arg(vl, uint64_t));
                            strcpy(&kprintf_buff[finalstrsz], tmp);
                            finalstrsz += strlen(tmp);
                            break;
                        }

                        // print hex value
                        case 'x':{
                            const char* tmp = nullptr;
                            tmp = utohexstr(va_arg(vl, int64_t));
                            strcpy(&kprintf_buff[finalstrsz], tmp);
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
                    strcpy(&kprintf_buff[finalstrsz], tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }

                // print integer
                case 'u':{
                    const char* tmp = nullptr;
                    tmp = utostr(va_arg(vl, uint32_t));
                    strcpy(&kprintf_buff[finalstrsz], tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }

                case 'x':{
                    const char* tmp = nullptr;
                    tmp = utohexstr(va_arg(vl, uint32_t));
                    strcpy(&kprintf_buff[finalstrsz], tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }

                case 's':{
                    char* tmp = va_arg(vl, char*);
                    strcat(kprintf_buff, tmp);
                    finalstrsz += strlen(tmp);
                    break;
                }
            }
        }else{
            kprintf_buff[finalstrsz] = fmtstr[i];
            finalstrsz++;
        }

        i++;
    }

    // null terminate final string
    kprintf_buff[finalstrsz] = 0;

    // print the string now with line wrap enabled
    DrawString(kprintf_buff, xpos, ypos, true);

    va_end(vl);
    return finalstrsz;
}
