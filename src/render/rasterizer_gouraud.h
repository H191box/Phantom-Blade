/*
 * Phantom Blade - Gouraud Shading Rasterizer
 * Per-vertex color interpolation across triangle faces
 * More expensive than flat shading but produces much smoother lighting
 */

#ifndef RASTERIZER_GOURAUD_H
#define RASTERIZER_GOURAUD_H

#include "gba_types.h"
#include "math3d.h"

/* Draw a gouraud-shaded triangle with per-vertex colors */
void draw_triangle_gouraud(ScreenVertex v0, ScreenVertex v1, ScreenVertex v2);

#endif /* RASTERIZER_GOURAUD_H */
