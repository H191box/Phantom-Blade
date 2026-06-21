/*
 * Phantom Blade - Triangle Rasterizer
 * Scanline-based flat-shaded triangle renderer
 * Core inner loop placed in IWRAM for maximum performance
 *
 * Algorithm:
 * 1. Sort vertices by Y (top to bottom)
 * 2. Walk edges with fixed-point stepping
 * 3. For each scanline, interpolate X between left and right edges
 * 4. Fill pixels between edges
 */

#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "gba_types.h"
#include "math3d.h"

/* Initialize rasterizer (set framebuffer pointer) */
void raster_init(void);

/* Set current drawing framebuffer */
void raster_set_buffer(u16* fb);

/* Draw a filled flat-shaded triangle to current buffer */
void draw_triangle_filled(ScreenVertex v0, ScreenVertex v1, ScreenVertex v2, u16 color);

/* Draw a horizontal line (fast inner loop) */
void draw_hline(s32 x0, s32 x1, s32 y, u16 color);

/* Draw a single pixel to current buffer */
void draw_pixel(s32 x, s32 y, u16 color);

/* Draw a rectangle (filled) */
void draw_rect(s32 x, s32 y, s32 w, s32 h, u16 color);

/* Draw a rectangle outline */
void draw_rect_outline(s32 x, s32 y, s32 w, s32 h, u16 color);

/* Clipping constants */
#define SCREEN_W  160
#define SCREEN_H  128

/* Performance counter (for debug) */
u32 raster_get_triangle_count(void);
void raster_reset_counter(void);

#endif /* RASTERIZER_H */
