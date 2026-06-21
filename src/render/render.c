/*
 * Phantom Blade - Main 3D Rendering System
 *
 * Complete rendering pipeline for GBA Mode 5 (160x128 16-bit color)
 *
 * Pipeline stages:
 * 1. Client submits triangles (world/model space)
 * 2. Transform by model matrix (model → world)
 * 3. Transform by view matrix (world → camera)
 * 4. Backface culling (skip back-facing triangles)
 * 5. Near-plane clipping
 * 6. Perspective projection (camera → screen)
 * 7. Depth sort (painter's algorithm, back-to-front)
 * 8. Rasterize to framebuffer
 */

#include "render.h"
#include "rasterizer.h"
#include "rasterizer_gouraud.h"
#include "gba_header.h"
#include "system.h"
#include "dma.h"
#include "math3d.h"
#include <string.h>

/* ========================================================================
 * Internal State
 * ======================================================================== */

/* Framebuffer pointer */
static u16* back_buffer = NULL;

/* Transform matrices */
static Mat4 model_matrix;
static Mat4 view_matrix;
static Mat4 mv_matrix;    /* model * view combined */

/* Projection */
static s32 focal_length = 160;  /* Focal length in pixels */

/* Triangle submission buffer */
static RenderTriangle tri_buffer[MAX_TRIANGLES_PER_FRAME];
static s32 tri_count = 0;

/* Stats */
static u32 stat_triangles = 0;
static u32 stat_culled = 0;

/* Near clip plane (in 8.8 fixed-point, ~1.0 unit) */
#define NEAR_CLIP  INT_TO_F88(1)

/* ========================================================================
 * Initialize
 * ======================================================================== */

void render_init(void) {
    back_buffer = sys_get_back_buffer();
    model_matrix = mat4_identity();
    view_matrix = mat4_identity();
    mv_matrix = mat4_identity();
    tri_count = 0;
    focal_length = 160;
    raster_init();
    raster_set_buffer(back_buffer);
    render_reset_stats();
}

/* ========================================================================
 * Framebuffer Management
 * ======================================================================== */

void render_clear(u16 color) {
    back_buffer = sys_get_back_buffer();
    raster_set_buffer(back_buffer);
    /* Fast DMA fill of entire back buffer */
    dma_fill16(back_buffer, color, MODE5_WIDTH * MODE5_HEIGHT);
}

u16* render_get_back_buffer(void) {
    return back_buffer;
}

/* ========================================================================
 * Transform Setup
 * ======================================================================== */

void render_set_model_matrix(Mat4 m) {
    model_matrix = m;
    /* Combine model * view for single-multiply transform */
    mv_matrix = mat4_multiply(model_matrix, view_matrix);
}

void render_set_view_matrix(Mat4 m) {
    view_matrix = m;
    mv_matrix = mat4_multiply(model_matrix, view_matrix);
}

void render_set_camera(Vec3 pos, Vec3 target, Vec3 up) {
    view_matrix = mat4_look_at(pos, target, up);
    mv_matrix = mat4_multiply(model_matrix, view_matrix);
}

void render_set_perspective(s32 fl) {
    focal_length = fl;
}

/* ========================================================================
 * Triangle Submission
 * ======================================================================== */

void render_triangle(Vec3 v0, Vec3 v1, Vec3 v2, u16 color) {
    if (tri_count >= MAX_TRIANGLES_PER_FRAME) return;

    /* Transform vertices to camera space */
    Vec3 cv0 = vec3_transform(v0, mv_matrix);
    Vec3 cv1 = vec3_transform(v1, mv_matrix);
    Vec3 cv2 = vec3_transform(v2, mv_matrix);

    /* Near-plane clipping: skip if ALL vertices behind near plane */
    if (cv0.z < NEAR_CLIP && cv1.z < NEAR_CLIP && cv2.z < NEAR_CLIP) {
        stat_culled++;
        return;
    }

    /* Backface culling using camera-space cross product */
    /* Check if triangle faces the camera (positive Z after transform) */
    Vec3 e1 = vec3_sub(cv1, cv0);
    Vec3 e2 = vec3_sub(cv2, cv0);
    /* Cross product Z component: e1.x*e2.y - e1.y*e2.x */
    s32 cross_z = (s32)(((s64)e1.x * e2.y - (s64)e1.y * e2.x) >> 8);
    if (cross_z <= 0) {
        stat_culled++;
        return;
    }

    /* Project to screen */
    RenderTriangle* rt = &tri_buffer[tri_count];
    project_vertex(cv0, SCREEN_W, SCREEN_H, focal_length, &rt->v[0]);
    project_vertex(cv1, SCREEN_W, SCREEN_H, focal_length, &rt->v[1]);
    project_vertex(cv2, SCREEN_W, SCREEN_H, focal_length, &rt->v[2]);

    /* Skip if any vertex failed projection */
    if (rt->v[0].x == -32768 || rt->v[1].x == -32768 || rt->v[2].x == -32768) {
        stat_culled++;
        return;
    }

    /* Compute average Z for sorting */
    rt->avg_z = (cv0.z + cv1.z + cv2.z) / 3;
    rt->color = color;
    rt->use_gouraud = FALSE;

    tri_count++;
    stat_triangles++;
}

void render_triangle_gouraud(Vec3 v0, Vec3 v1, Vec3 v2,
                              u16 c0, u16 c1, u16 c2) {
    if (tri_count >= MAX_TRIANGLES_PER_FRAME) return;

    Vec3 cv0 = vec3_transform(v0, mv_matrix);
    Vec3 cv1 = vec3_transform(v1, mv_matrix);
    Vec3 cv2 = vec3_transform(v2, mv_matrix);

    if (cv0.z < NEAR_CLIP && cv1.z < NEAR_CLIP && cv2.z < NEAR_CLIP) {
        stat_culled++;
        return;
    }

    Vec3 e1 = vec3_sub(cv1, cv0);
    Vec3 e2 = vec3_sub(cv2, cv0);
    s32 cross_z = (s32)(((s64)e1.x * e2.y - (s64)e1.y * e2.x) >> 8);
    if (cross_z <= 0) {
        stat_culled++;
        return;
    }

    RenderTriangle* rt = &tri_buffer[tri_count];
    project_vertex(cv0, SCREEN_W, SCREEN_H, focal_length, &rt->v[0]);
    project_vertex(cv1, SCREEN_W, SCREEN_H, focal_length, &rt->v[1]);
    project_vertex(cv2, SCREEN_W, SCREEN_H, focal_length, &rt->v[2]);

    if (rt->v[0].x == -32768 || rt->v[1].x == -32768 || rt->v[2].x == -32768) {
        stat_culled++;
        return;
    }

    rt->avg_z = (cv0.z + cv1.z + cv2.z) / 3;
    rt->v[0].color = c0;
    rt->v[1].color = c1;
    rt->v[2].color = c2;
    rt->color = c0;
    rt->use_gouraud = TRUE;

    tri_count++;
    stat_triangles++;
}

/* ========================================================================
 * Depth Sort (Painter's Algorithm) - Insertion Sort
 * Sort back-to-front (largest Z first = farthest first)
 * ======================================================================== */

static void sort_triangles_back_to_front(void) {
    s32 i, j;
    for (i = 1; i < tri_count; i++) {
        RenderTriangle key = tri_buffer[i];
        j = i - 1;
        while (j >= 0 && tri_buffer[j].avg_z < key.avg_z) {
            tri_buffer[j + 1] = tri_buffer[j];
            j--;
        }
        tri_buffer[j + 1] = key;
    }
}

/* ========================================================================
 * Flush - Depth Sort and Rasterize All Triangles
 * ======================================================================== */

void render_flush(void) {
    /* Sort by depth (back to front) */
    sort_triangles_back_to_front();

    /* Rasterize each triangle */
    raster_reset_counter();
    s32 i;
    for (i = 0; i < tri_count; i++) {
        RenderTriangle* rt = &tri_buffer[i];
        if (rt->use_gouraud) {
            draw_triangle_gouraud(rt->v[0], rt->v[1], rt->v[2]);
        } else {
            draw_triangle_filled(rt->v[0], rt->v[1], rt->v[2], rt->color);
        }
    }

    /* Reset triangle buffer for next frame */
    tri_count = 0;
}

/* ========================================================================
 * 2D Drawing Helpers
 * ======================================================================== */

void render_draw_rect(s32 x, s32 y, s32 w, s32 h, u16 color) {
    draw_rect(x, y, w, h, color);
}

void render_draw_rect_outline(s32 x, s32 y, s32 w, s32 h, u16 color) {
    draw_rect_outline(x, y, w, h, color);
}

void render_hud(void) {
    /* HUD rendering is handled by hud.c */
    /* This is a placeholder for post-scene 2D overlay */
}

/* ========================================================================
 * Stats
 * ======================================================================== */

u32 render_get_stats_triangles(void) { return stat_triangles; }
u32 render_get_stats_culled(void)    { return stat_culled; }

void render_reset_stats(void) {
    stat_triangles = 0;
    stat_culled = 0;
}
