/*
 * Phantom Blade - Sprite Overlay Implementation
 * 2D primitives and text rendering on top of 3D scene
 */

#include "sprite_overlay.h"
#include "font.h"
#include "render.h"
#include "gba_header.h"
#include "dma.h"

static s32 fade_level = 16;  /* 0=black, 16=full brightness */

void sprite_init(void) {
    fade_level = 16;
}

void sprite_draw_rect(s32 x, s32 y, s32 w, s32 h, u16 color) {
    render_draw_rect_outline(x, y, w, h, color);
}

void sprite_draw_rect_filled(s32 x, s32 y, s32 w, s32 h, u16 color) {
    render_draw_rect(x, y, w, h, color);
}

/*
 * Draw a progress bar (HP bar, XP bar, etc.)
 */
void sprite_draw_bar(s32 x, s32 y, s32 w, s32 h,
                     s32 current, s32 max_val,
                     u16 bg_color, u16 fill_color, u16 border_color) {
    /* Background */
    render_draw_rect(x, y, w, h, bg_color);
    /* Fill */
    s32 fill_w = 0;
    if (max_val > 0) {
        fill_w = (current * (w - 2)) / max_val;
    }
    if (fill_w > 0) {
        render_draw_rect(x + 1, y + 1, fill_w, h - 2, fill_color);
    }
    /* Border */
    render_draw_rect_outline(x, y, w, h, border_color);
}

/*
 * Draw text string using 8x8 font
 */
void sprite_draw_text(s32 x, s32 y, const char* str, u16 color) {
    s32 cx = x, cy = y;
    while (*str) {
        if (*str == '\n') {
            cx = x;
            cy += 8;
        } else if (*str >= 32) {
            font_draw_char(cx, cy, *str, color);
            cx += 8;
        }
        str++;
    }
}

void sprite_draw_text_shadow(s32 x, s32 y, const char* str,
                              u16 color, u16 shadow_color) {
    /* Draw shadow offset by 1 pixel */
    sprite_draw_text(x + 1, y + 1, str, shadow_color);
    sprite_draw_text(x, y, str, color);
}

void sprite_draw_char(s32 x, s32 y, char ch, u16 color) {
    font_draw_char(x, y, ch, color);
}

void sprite_draw_icon(s32 x, s32 y, u32 icon_id, u16 color) {
    /* Simple icon shapes */
    switch (icon_id) {
        case 0: /* Heart (HP) */
            render_draw_rect(x+2, y, 4, 2, color);
            render_draw_rect(x, y+2, 8, 2, color);
            render_draw_rect(x+2, y+4, 4, 2, color);
            render_draw_rect(x+4, y+6, 2, 2, color);
            break;
        case 1: /* Star (XP) */
            render_draw_rect(x+2, y, 4, 2, color);
            render_draw_rect(x, y+2, 8, 2, color);
            render_draw_rect(x+2, y+4, 4, 2, color);
            break;
        case 2: /* Key */
            render_draw_rect(x+2, y, 2, 3, color);
            render_draw_rect(x+4, y+2, 4, 2, color);
            render_draw_rect(x+6, y+4, 2, 2, color);
            break;
        default:
            render_draw_rect(x, y, 8, 8, color);
            break;
    }
}

/*
 * Full-screen fade effects
 */
void sprite_fade_out(void) {
    /* Progressive fade to black using DMA overlay */
    u16* fb = render_get_back_buffer();
    /* Simple approach: reduce all pixel brightness */
    /* For speed, just dim every other frame */
    s32 step;
    for (step = 15; step >= 0; step--) {
        s32 i;
        for (i = 0; i < MODE5_WIDTH * MODE5_HEIGHT; i++) {
            u16 px = fb[i];
            s32 r = px & 0x1F;
            s32 g = (px >> 5) & 0x1F;
            s32 b = (px >> 10) & 0x1F;
            r = (r * step) >> 4;
            g = (g * step) >> 4;
            b = (b * step) >> 4;
            fb[i] = (b << 10) | (g << 5) | r;
        }
        fade_level = step;
    }
}

void sprite_fade_in(void) {
    fade_level = 16;
}

s32 sprite_get_fade_level(void) {
    return fade_level;
}

void sprite_set_fade_level(s32 level) {
    fade_level = CLAMP(level, 0, 16);
}
