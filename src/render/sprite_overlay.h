/*
 * Phantom Blade - Sprite Overlay System
 * 2D sprites drawn on top of the 3D framebuffer
 * Handles HUD elements, text, and UI overlays
 */

#ifndef SPRITE_OVERLAY_H
#define SPRITE_OVERLAY_H

#include "gba_types.h"
#include "math3d.h"

/* Initialize sprite overlay system */
void sprite_init(void);

/* Draw a colored rectangle at screen coords */
void sprite_draw_rect(s32 x, s32 y, s32 w, s32 h, u16 color);
void sprite_draw_rect_filled(s32 x, s32 y, s32 w, s32 h, u16 color);

/* Draw a horizontal bar (for HP, XP gauges) */
void sprite_draw_bar(s32 x, s32 y, s32 w, s32 h,
                     s32 current, s32 max_val,
                     u16 bg_color, u16 fill_color, u16 border_color);

/* Draw text string at screen position */
void sprite_draw_text(s32 x, s32 y, const char* str, u16 color);

/* Draw text with shadow for readability */
void sprite_draw_text_shadow(s32 x, s32 y, const char* str,
                             u16 color, u16 shadow_color);

/* Draw a single character */
void sprite_draw_char(s32 x, s32 y, char ch, u16 color);

/* Draw a simple icon (weapon, item, etc.) */
void sprite_draw_icon(s32 x, s32 y, u32 icon_id, u16 color);

/* Full screen fade to black */
void sprite_fade_out(void);

/* Full screen fade from black */
void sprite_fade_in(void);

/* Get/set fade level (0=black, 16=full) */
s32 sprite_get_fade_level(void);
void sprite_set_fade_level(s32 level);

#endif /* SPRITE_OVERLAY_H */
