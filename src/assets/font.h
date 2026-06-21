/*
 * Phantom Blade - Font System
 * 8x8 pixel font for framebuffer text rendering
 */

#ifndef FONT_H
#define FONT_H

#include "gba_types.h"

/* Initialize font system */
void font_init(void);

/* Draw a single character at screen position */
void font_draw_char(s32 x, s32 y, char ch, u16 color);

/* Draw string */
void font_draw_string(s32 x, s32 y, const char* str, u16 color);

#endif /* FONT_H */
