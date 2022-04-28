/**
 * @file FontData.cpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 04/28/22
 * @modified 04/28/22
 * @brief Provides a font bitmap for the kernel to use.
 * @copyright MIT License 2022 Siddharth Mishra
 * */


#ifndef FONTDATA_H_
#define FONTDATA_H_

#include <cstdint>

/// width of font in pixels.
extern uint8_t FONT_WIDTH;
/// height of font in pixels.
extern uint8_t FONT_HEIGHT;

/// font bitmap data array.
extern uint8_t* FONT_DATA;

#endif // FONTDATA_H_
