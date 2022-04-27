#include "Renderer.hpp"
#include "FontData.hpp"
#include "String.hpp"

// framebuffer info
uint32_t fbWidth = 0;
uint32_t fbHeight = 0;
uint32_t fbPitch = 0;
uint32_t* framebuffer = 0;

// load framebuffer info
void LoadFramebufferInfo(stivale2_struct_tag_framebuffer* fb_tag){
    fbWidth = fb_tag->framebuffer_width;
    fbHeight = fb_tag->framebuffer_height;
    fbPitch = fb_tag->framebuffer_pitch;
    framebuffer = reinterpret_cast<uint32_t*>(fb_tag->framebuffer_addr);
}

// clear a rectangle on sreen with given color
void ClearScreen(uint32_t color,
                 uint32_t startx, uint32_t starty,
                 uint32_t stopx, uint32_t stopy){
    for(uint32_t r = starty; r <= stopy; r++){
        for(uint32_t c = startx; c <= stopx; c++){
            framebuffer[r * fbWidth + c] = color;
        }
    }
}

// draw character on screen at given posn
void DrawCharacter(char c, uint32_t x, uint32_t y,
                   uint32_t fgColor, uint32_t bgColor){
    // get bitmap for required character
    uint8_t* font_bitmap = FONT_DATA + c * FONT_HEIGHT;

    // draw char
    for(uint32_t i = 0; i < FONT_HEIGHT; i++){
        uint8_t row_bitmap = font_bitmap[i];
        for(uint32_t j = 0; j < FONT_HEIGHT; j++){
            // calculate write address
            uint32_t write_addr = (x + j) + (y + i) * fbWidth;

            if(c == '\b'){
                framebuffer[write_addr] = bgColor;
            }else {
                // if bit is set then fill foreground colour else fill background colour
                // 8 - j because of endianness of pc when compiling. Bits are stored in little endian format
                if(row_bitmap & (1 << (8 - j))){
                    framebuffer[write_addr] = fgColor;
                }else{
                    framebuffer[write_addr] = bgColor;
                }
            }
        }
    }
};

// TODO: add line wrap support
void DrawString(const char* str, uint32_t x, uint32_t y, bool wrap){
    size_t len = StringLength(str);
    for(size_t i = 0; i < len; i++){
        DrawCharacter(str[i], x + i*FONT_WIDTH, y);
    }
}
