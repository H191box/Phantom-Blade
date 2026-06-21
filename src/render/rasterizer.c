/*
 * Phantom Blade - Triangle Rasterizer Implementation
 * Scanline flat-shaded triangle rendering for GBA Mode 5
 *
 * Performance-critical code uses IWRAM section for speed.
 * Screen coordinates are in 8.8 fixed-point for sub-pixel accuracy.
 * Drawing coordinates are integer (pixel) after conversion.
 */

#include "rasterizer.h"
#include "gba_header.h"

/* Current framebuffer pointer */
static u16* fb = NULL;

/* Triangle count for profiling */
static u32 tri_count = 0;

void raster_init(void) {
    fb = NULL;
    tri_count = 0;
}

void raster_set_buffer(u16* buffer) {
    fb = buffer;
}

u32 raster_get_triangle_count(void) {
    return tri_count;
}

void raster_reset_counter(void) {
    tri_count = 0;
}

/*
 * Draw a single pixel - inline for speed
 * Bounds-checked for safety
 */
void draw_pixel(s32 x, s32 y, u16 color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
    fb[y * SCREEN_W + x] = color;
}

/*
 * Draw horizontal line - the core fill primitive
 * x0, x1: integer pixel coordinates
 * y: integer pixel coordinate
 * Fast: just a linear fill
 */
void draw_hline(s32 x0, s32 x1, s32 y, u16 color) {
    /* Clamp to screen bounds */
    if (y < 0 || y >= SCREEN_H) return;
    if (x0 < 0) x0 = 0;
    if (x1 >= SCREEN_W) x1 = SCREEN_W - 1;
    if (x0 > x1) return;

    u16* line = fb + y * SCREEN_W;
    s32 i;
    for (i = x0; i <= x1; i++) {
        line[i] = color;
    }
}

/*
 * Draw filled rectangle
 */
void draw_rect(s32 x, s32 y, s32 w, s32 h, u16 color) {
    s32 i;
    s32 x0 = x, x1 = x + w - 1;
    s32 y0 = y, y1 = y + h - 1;
    if (x0 < 0) x0 = 0;
    if (x1 >= SCREEN_W) x1 = SCREEN_W - 1;
    if (y0 < 0) y0 = 0;
    if (y1 >= SCREEN_H) y1 = SCREEN_H - 1;
    for (i = y0; i <= y1; i++) {
        draw_hline(x0, x1, i, color);
    }
}

/*
 * Draw rectangle outline
 */
void draw_rect_outline(s32 x, s32 y, s32 w, s32 h, u16 color) {
    s32 i;
    draw_hline(x, x + w - 1, y, color);
    draw_hline(x, x + w - 1, y + h - 1, color);
    for (i = y; i < y + h; i++) {
        draw_pixel(x, i, color);
        draw_pixel(x + w - 1, i, color);
    }
}

/*
 * Draw filled triangle - scanline rasterization
 *
 * Input vertices have x,y in 8.8 fixed-point (screen coords).
 * We convert to integer pixel coords for filling.
 *
 * Algorithm:
 * 1. Sort vertices by Y coordinate (ascending)
 * 2. Calculate edge slopes using fixed-point stepping
 * 3. Walk from top to bottom, interpolating X on each edge
 * 4. Fill horizontal spans between left and right edges
 */
void draw_triangle_filled(ScreenVertex sv0, ScreenVertex sv1,
                          ScreenVertex sv2, u16 color) {
    if (fb == NULL) return;

    tri_count++;

    /*
     * Convert 8.8 screen coordinates to integer pixels
     * (round to nearest by adding 128 = 0.5 in 8.8)
     */
    s32 x0 = (sv0.x + 128) >> 8;
    s32 y0 = (sv0.y + 128) >> 8;
    s32 x1 = (sv1.x + 128) >> 8;
    s32 y1 = (sv1.y + 128) >> 8;
    s32 x2 = (sv2.x + 128) >> 8;
    s32 y2 = (sv2.y + 128) >> 8;

    /* Quick bounding box reject */
    s32 min_y = MIN(y0, MIN(y1, y2));
    s32 max_y = MAX(y0, MAX(y1, y2));
    s32 min_x = MIN(x0, MIN(x1, x2));
    s32 max_x = MAX(x0, MAX(x1, x2));

    if (max_y < 0 || min_y >= SCREEN_H) return;
    if (max_x < 0 || min_x >= SCREEN_W) return;

    /* Sort vertices by Y (ascending) */
    /* After sorting: va has smallest Y, vc has largest Y */
    s32 ax, ay, bx, by, cx, cy;
    if (y0 <= y1 && y0 <= y2) {
        /* a = v0 */
        ax = x0; ay = y0;
        if (y1 <= y2) { bx = x1; by = y1; cx = x2; cy = y2; }
        else           { bx = x2; by = y2; cx = x1; cy = y1; }
    } else if (y1 <= y0 && y1 <= y2) {
        /* a = v1 */
        ax = x1; ay = y1;
        if (y0 <= y2) { bx = x0; by = y0; cx = x2; cy = y2; }
        else           { bx = x2; by = y2; cx = x0; cy = y0; }
    } else {
        /* a = v2 */
        ax = x2; ay = y2;
        if (y0 <= y1) { bx = x0; by = y0; cx = x1; cy = y1; }
        else           { bx = x1; by = y1; cx = x0; cy = y0; }
    }

    /* Degenerate triangle (all same Y or height = 0) */
    if (cy <= ay) return;

    /*
     * Scanline rasterization with fixed-point edge interpolation
     * Use 12.4 fixed-point for X edge stepping (4 bits sub-pixel)
     */
    #define FP_SHIFT 4
    #define FP_ONE  (1 << FP_SHIFT)

    s32 edge_height_total = cy - ay;

    /* Long edge: from vertex A (top) to vertex C (bottom) */
    /* Fixed-point X stepping */
    s32 long_x = ax << FP_SHIFT;
    s32 long_dx = (edge_height_total > 0) ?
                  ((cx - ax) << FP_SHIFT) / edge_height_total : 0;

    /* First half: from A to B */
    s32 height_ab = by - ay;
    if (height_ab > 0) {
        s32 short_x = ax << FP_SHIFT;
        s32 short_dx = ((bx - ax) << FP_SHIFT) / height_ab;

        s32 scan_y;
        for (scan_y = ay; scan_y < by; scan_y++) {
            if (scan_y >= 0 && scan_y < SCREEN_H) {
                s32 lx = long_x >> FP_SHIFT;
                s32 rx = short_x >> FP_SHIFT;
                if (lx > rx) { s32 t = lx; lx = rx; rx = t; }
                draw_hline(lx, rx, scan_y, color);
            }
            long_x += long_dx;
            short_x += short_dx;
        }
    }

    /* Second half: from B to C */
    s32 height_bc = cy - by;
    if (height_bc > 0) {
        s32 short_x = bx << FP_SHIFT;
        s32 short_dx = ((cx - bx) << FP_SHIFT) / height_bc;

        s32 scan_y;
        for (scan_y = by; scan_y <= cy; scan_y++) {
            if (scan_y >= 0 && scan_y < SCREEN_H) {
                s32 lx = long_x >> FP_SHIFT;
                s32 rx = short_x >> FP_SHIFT;
                if (lx > rx) { s32 t = lx; lx = rx; rx = t; }
                draw_hline(lx, rx, scan_y, color);
            }
            long_x += long_dx;
            short_x += short_dx;
        }
    }

    #undef FP_SHIFT
    #undef FP_ONE
}
