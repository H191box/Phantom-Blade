/*
 * Phantom Blade - Gouraud Shading Rasterizer
 * Interpolates RGB across triangle scanlines from per-vertex colors
 *
 * Color interpolation uses 4.12 fixed-point for smooth gradients
 * Only used for player model and important objects (flat shading for walls/floor)
 */

#include "rasterizer_gouraud.h"
#include "rasterizer.h"
#include "gba_header.h"

extern u16* raster_fb;

/* Helper: unpack RGB555 to separate channels */
static inline void unpack_color(u16 c, s32* r, s32* g, s32* b) {
    *r = c & 0x1F;
    *g = (c >> 5) & 0x1F;
    *b = (c >> 10) & 0x1F;
}

/* Helper: pack RGB channels back to RGB555 */
static inline u16 pack_color(s32 r, s32 g, s32 b) {
    return (u16)((CLAMP(b, 0, 31) << 10) |
                  (CLAMP(g, 0, 31) << 5) |
                  CLAMP(r, 0, 31));
}

/*
 * Gouraud triangle: same scanline approach as flat, but interpolate colors
 * across each scanline from left-edge color to right-edge color.
 */
void draw_triangle_gouraud(ScreenVertex sv0, ScreenVertex sv1, ScreenVertex sv2) {
    extern u16* fb;
    if (fb == NULL) return;

    /* Convert 8.8 screen coords to integer pixels */
    s32 x0 = (sv0.x + 128) >> 8;
    s32 y0 = (sv0.y + 128) >> 8;
    s32 x1 = (sv1.x + 128) >> 8;
    s32 y1 = (sv1.y + 128) >> 8;
    s32 x2 = (sv2.x + 128) >> 8;
    s32 y2 = (sv2.y + 128) >> 8;

    /* Bounding box reject */
    s32 min_y = MIN(y0, MIN(y1, y2));
    s32 max_y = MAX(y0, MAX(y1, y2));
    if (max_y < 0 || min_y >= SCREEN_H) return;

    /* Sort by Y */
    s32 ax, ay, bx, by, cx, cy;
    u16 ca, cb, cc;  /* vertex colors */
    if (y0 <= y1 && y0 <= y2) {
        ax = x0; ay = y0; ca = sv0.color;
        if (y1 <= y2) { bx = x1; by = y1; cb = sv1.color; cx = x2; cy = y2; cc = sv2.color; }
        else           { bx = x2; by = y2; cb = sv2.color; cx = x1; cy = y1; cc = sv1.color; }
    } else if (y1 <= y0 && y1 <= y2) {
        ax = x1; ay = y1; ca = sv1.color;
        if (y0 <= y2) { bx = x0; by = y0; cb = sv0.color; cx = x2; cy = y2; cc = sv2.color; }
        else           { bx = x2; by = y2; cb = sv2.color; cx = x0; cy = y0; cc = sv0.color; }
    } else {
        ax = x2; ay = y2; ca = sv2.color;
        if (y0 <= y1) { bx = x0; by = y0; cb = sv0.color; cx = x1; cy = y1; cc = sv1.color; }
        else           { bx = x1; by = y1; cb = sv1.color; cx = x0; cy = y0; cc = sv0.color; }
    }

    if (cy <= ay) return;

    /* Unpack vertex colors */
    s32 ar, ag, ab_r; unpack_color(ca, &ar, &ag, &ab_r);
    s32 br, bg, bb;   unpack_color(cb, &br, &bg, &bb);
    s32 cr, cg, cb_c; unpack_color(cc, &cr, &cg, &cb_c);

    #define FP_SHIFT 4
    #define FP_ONE  (1 << FP_SHIFT)
    #define COLOR_SHIFT 6  /* 6.5 fixed-point for color interpolation */

    s32 edge_total = cy - ay;

    /* Long edge (A to C) */
    s32 long_x = ax << FP_SHIFT;
    s32 long_dx = (edge_total > 0) ? ((cx - ax) << FP_SHIFT) / edge_total : 0;
    s32 long_r = ar << COLOR_SHIFT;
    s32 long_dr = (edge_total > 0) ? ((cr - ar) << COLOR_SHIFT) / edge_total : 0;
    s32 long_g = ag << COLOR_SHIFT;
    s32 long_dg = (edge_total > 0) ? ((cg - ag) << COLOR_SHIFT) / edge_total : 0;
    s32 long_b = ab_r << COLOR_SHIFT;
    s32 long_db = (edge_total > 0) ? ((cb_c - ab_r) << COLOR_SHIFT) / edge_total : 0;

    /* First half (A to B) */
    s32 h1 = by - ay;
    if (h1 > 0) {
        s32 short_x = ax << FP_SHIFT;
        s32 short_dx = ((bx - ax) << FP_SHIFT) / h1;
        s32 short_r = ar << COLOR_SHIFT;
        s32 short_dr = ((br - ar) << COLOR_SHIFT) / h1;
        s32 short_g = ag << COLOR_SHIFT;
        s32 short_dg = ((bg - ag) << COLOR_SHIFT) / h1;
        s32 short_b = ab_r << COLOR_SHIFT;
        s32 short_db = ((bb - ab_r) << COLOR_SHIFT) / h1;

        s32 scan_y;
        for (scan_y = ay; scan_y < by; scan_y++) {
            if (scan_y >= 0 && scan_y < SCREEN_H) {
                s32 lx = long_x >> FP_SHIFT;
                s32 rx = short_x >> FP_SHIFT;
                if (lx > rx) { s32 t = lx; lx = rx; rx = t; }

                if (rx >= 0 && lx < SCREEN_W) {
                    if (lx < 0) lx = 0;
                    if (rx >= SCREEN_W) rx = SCREEN_W - 1;

                    s32 span = rx - lx;
                    if (span > 0) {
                        s32 lr = long_r >> COLOR_SHIFT;
                        s32 lg = long_g >> COLOR_SHIFT;
                        s32 lb = long_b >> COLOR_SHIFT;
                        s32 sr = short_r >> COLOR_SHIFT;
                        s32 sg = short_g >> COLOR_SHIFT;
                        s32 sb = short_b >> COLOR_SHIFT;

                        s32 dr = span > 0 ? ((sr - lr) << COLOR_SHIFT) / span : 0;
                        s32 dg = span > 0 ? ((sg - lg) << COLOR_SHIFT) / span : 0;
                        s32 db = span > 0 ? ((sb - lb) << COLOR_SHIFT) / span : 0;

                        s32 cr_int = lr << COLOR_SHIFT;
                        s32 cg_int = lg << COLOR_SHIFT;
                        s32 cb_int = lb << COLOR_SHIFT;
                        u16* line = fb + scan_y * SCREEN_W;
                        s32 px;
                        for (px = lx; px <= rx; px++) {
                            line[px] = pack_color(cr_int >> COLOR_SHIFT,
                                                  cg_int >> COLOR_SHIFT,
                                                  cb_int >> COLOR_SHIFT);
                            cr_int += dr;
                            cg_int += dg;
                            cb_int += db;
                        }
                    }
                }
            }
            long_x += long_dx; short_x += short_dx;
            long_r += long_dr; short_r += short_dr;
            long_g += long_dg; short_g += short_dg;
            long_b += long_db; short_b += short_db;
        }
    }

    /* Second half (B to C) */
    s32 h2 = cy - by;
    if (h2 > 0) {
        s32 short_x = bx << FP_SHIFT;
        s32 short_dx = ((cx - bx) << FP_SHIFT) / h2;
        s32 short_r = br << COLOR_SHIFT;
        s32 short_dr = ((cr - br) << COLOR_SHIFT) / h2;
        s32 short_g = bg << COLOR_SHIFT;
        s32 short_dg = ((cg - bg) << COLOR_SHIFT) / h2;
        s32 short_b = bb << COLOR_SHIFT;
        s32 short_db = ((cb_c - bb) << COLOR_SHIFT) / h2;

        s32 scan_y;
        for (scan_y = by; scan_y <= cy; scan_y++) {
            if (scan_y >= 0 && scan_y < SCREEN_H) {
                s32 lx = long_x >> FP_SHIFT;
                s32 rx = short_x >> FP_SHIFT;
                if (lx > rx) { s32 t = lx; lx = rx; rx = t; }

                if (rx >= 0 && lx < SCREEN_W) {
                    if (lx < 0) lx = 0;
                    if (rx >= SCREEN_W) rx = SCREEN_W - 1;

                    s32 span = rx - lx;
                    if (span > 0) {
                        s32 lr = long_r >> COLOR_SHIFT;
                        s32 lg = long_g >> COLOR_SHIFT;
                        s32 lb = long_b >> COLOR_SHIFT;
                        s32 sr = short_r >> COLOR_SHIFT;
                        s32 sg = short_g >> COLOR_SHIFT;
                        s32 sb = short_b >> COLOR_SHIFT;

                        s32 dr = span > 0 ? ((sr - lr) << COLOR_SHIFT) / span : 0;
                        s32 dg = span > 0 ? ((sg - lg) << COLOR_SHIFT) / span : 0;
                        s32 db = span > 0 ? ((sb - lb) << COLOR_SHIFT) / span : 0;

                        s32 cr_int = lr << COLOR_SHIFT;
                        s32 cg_int = lg << COLOR_SHIFT;
                        s32 cb_int = lb << COLOR_SHIFT;
                        u16* line = fb + scan_y * SCREEN_W;
                        s32 px;
                        for (px = lx; px <= rx; px++) {
                            line[px] = pack_color(cr_int >> COLOR_SHIFT,
                                                  cg_int >> COLOR_SHIFT,
                                                  cb_int >> COLOR_SHIFT);
                            cr_int += dr;
                            cg_int += dg;
                            cb_int += db;
                        }
                    }
                }
            }
            long_x += long_dx; short_x += short_dx;
            long_r += long_dr; short_r += short_dr;
            long_g += long_dg; short_g += short_dg;
            long_b += long_db; short_b += short_db;
        }
    }

    #undef FP_SHIFT
    #undef FP_ONE
    #undef COLOR_SHIFT
}
