/**
 * @file Renderer.cpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 04/27/22
 * @modified 04/28/22
 * @brief Rendering functions. Provides basic rendering support to kernel.
 * @copyright MIT License 2022 Siddharth Mishra
 * */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "Common.hpp"
#include "stivale2.h"

#define DEFAULT_FGCOLOR 0xffffffff
#define DEFAULT_BGCOLOR 0

// export framebuffer information
extern u32 FRAMEBUFFER_WIDTH;
extern u32 FRAMEBUFFER_HEIGHT;
extern u32 FRAMEBUFFER_PITCH;

/**
 * @brief Helper function to generate color value from
 * given color components.
 *
 * @param r Red color component (0-255)
 * @param g Green color component (0-255)
 * @param b Blue color component (0-255)
 * @param a Alpha component (0-255)
 * */
inline u32 Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    return r << 24 | g << 16 | b << 8 | a;
}

/**
 * @brief Loads framebuffer information from framebuffer tag
 * provided by bootloader. This function must be called before
 * calling any render function. Once set, renderer will use the
 * loaded information for drawing objects onto screen.
 *
 * @param fb_tag Framebuffer tag provided by stivale2 compliant
 * bootloader.
 * */
void LoadFramebufferInfo(stivale2_struct_tag_framebuffer* fb_tag);

/**
 * @brief Clear the screen with given color for given rectangle.
 * By default, this will clear complete screen with black color.
 *
 * @param color Color to clear screen with.
 * @param startx Starting X Coordinate of rectangle.
 * @param starty Starting Y Coordinate of rectangle.
 * @param stopx Stopping X Coordinate of rectangle. 0 Means complete width.
 * @param stopy Stopping Y Coordinate of rectangle. 0 Means complete height.
 * */
void ClearScreen(u32 color = 0,
                 u32 startx = 0, u32 starty = 0,
                 u32 stopx = 0, u32 stopy = 0);

/**
 * @brief Draw a character on screen.
 *
 * @param c Character to draw onto screen.
 * @param x X Coordintate of where to draw on screen.
 * @param y Y Coordinate of where to draw on screen.
 * @param fgColor Foreground color. Color of text.
 * @param bgColor Background Color. Color of background of text.
 * */
void DrawCharacter(char c, u32& x, u32& y,
                   u32 fgColor = Color(0xff, 0xff, 0xff, 0xff), // white
                   u32 bgColor = Color(0, 0, 0, 0)); // black

/**
 * @brief Draw string on screen. Note that this function takes x and y
 * positions as reference and hence is capable of changing their values.
 * This function will automatically change value of x and y
 *
 * @param string String to draw onto screen
 * @param x X coordinate of where to start drawing from.
 * @param y Y coordinate of where to start drawing from.
 * @param fgColor Foreground color of string drawn.
 * @param bgColor Background color of string drawn.
 * */
void DrawString(const char* string, u32& x, u32& y,
                u32 fgColor = DEFAULT_FGCOLOR,
                u32 bgColor = DEFAULT_BGCOLOR);

#endif // RENDERER_H_
