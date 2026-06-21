/*
 * Phantom Blade - Main 3D Rendering System
 * Complete pipeline: model transform → view transform → backface cull →
 *                    near-clip → project → depth sort → rasterize
 */

#ifndef RENDER_H
#define RENDER_H

#include "gba_types.h"
#include "math3d.h"

/* ========================================================================
 * Render Pipeline Triangle
 * A triangle after full transform, ready for rasterization
 * ======================================================================== */
typedef struct {
    ScreenVertex v[3];  /* 3 projected screen vertices */
    s32 avg_z;           /* Average depth for sorting */
    u16 color;           /* Flat-shaded color */
    bool use_gouraud;    /* Use per-vertex color interpolation */
} RenderTriangle;

/* Maximum triangles per frame */
#define MAX_TRIANGLES_PER_FRAME  256

/* ========================================================================
 * Render System API
 * ======================================================================== */

/* Initialize render system */
void render_init(void);

/* Clear back buffer to given color (DMA fast fill) */
void render_clear(u16 color);

/* Set the model->world transform matrix */
void render_set_model_matrix(Mat4 m);

/* Set the camera (view matrix) */
void render_set_view_matrix(Mat4 m);

/* Set camera from position and look-at target */
void render_set_camera(Vec3 pos, Vec3 target, Vec3 up);

/* Submit a single world-space triangle for rendering
 * Vertices should be in model space; will be transformed by model matrix */
void render_triangle(Vec3 v0, Vec3 v1, Vec3 v2, u16 color);

/* Submit a gouraud-shaded triangle */
void render_triangle_gouraud(Vec3 v0, Vec3 v1, Vec3 v2, u16 c0, u16 c1, u16 c2);

/* Flush all submitted triangles (depth sort + rasterize) */
void render_flush(void);

/* Render the HUD after 3D scene */
void render_hud(void);

/* Draw a 2D rectangle on the framebuffer */
void render_draw_rect(s32 x, s32 y, s32 w, s32 h, u16 color);
void render_draw_rect_outline(s32 x, s32 y, s32 w, s32 h, u16 color);

/* Get framebuffer pointer for direct access */
u16* render_get_back_buffer(void);

/* Perspective projection settings */
void render_set_perspective(s32 focal_length);

/* Debug: get stats */
u32 render_get_stats_triangles(void);
u32 render_get_stats_culled(void);
void render_reset_stats(void);

#endif /* RENDER_H */
