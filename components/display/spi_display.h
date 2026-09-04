#pragma once
#include <stdint.h>

#include <u8g2.h>


//--------------------------------------------------
typedef struct {
    uint8_t font_height;
    uint8_t font_width;
    const uint8_t *font;
} U8G2_FONT;


//--------------------------------------------------
extern const uint32_t SCREEN_HEIGHT;
extern const uint32_t SCREEN_WIDTH;

extern U8G2_FONT font_small;
extern U8G2_FONT font_big;


//--------------------------------------------------
void init_u8g2();
void display_text(char *buffer, uint32_t pos_x, uint32_t pos_y, U8G2_FONT *font, char *tag_logging);
