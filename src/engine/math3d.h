/*
 * Phantom Blade - 3D Math Library
 * Fixed-point vector, matrix, and transform operations
 * Hot functions placed in IWRAM for maximum performance
 */

#ifndef MATH3D_H
#define MATH3D_H

#include "gba_types.h"

/* ========================================================================
 * 3D Vector (fixed-point 8.8 format)
 * ======================================================================== */
typedef struct {
    s32 x, y, z;
} Vec3;

/* ========================================================================
 * 4x4 Matrix (fixed-point 8.8 format)
 * Stored as row-major: m[row][col]
 * For vertex transform: result = M * v (matrix * column vector)
 * ======================================================================== */
typedef struct {
    s32 m[4][4];
} Mat4;

/* ========================================================================
 * Screen-space vertex (after projection)
 * ======================================================================== */
typedef struct {
    s32 x, y;      /* Screen coordinates (pixel, 8.8 fixed) */
    s32 z;         /* Depth for sorting (32-bit integer) */
    u16 color;     /* RGB555 color */
} ScreenVertex;

/* ========================================================================
 * Transformed vertex (after model/view transform, before projection)
 * ======================================================================== */
typedef struct {
    s32 x, y, z;   /* World/camera space coords (8.8 fixed) */
    s32 color;      /* Lighting result */
} TransformVertex;

/* ========================================================================
 * Vector Operations (all in IWRAM)
 * ======================================================================== */
Vec3 vec3_make(s32 x, s32 y, s32 z);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 v, s32 s);
s32  vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_cross(Vec3 a, Vec3 b);
Vec3 vec3_normalize(Vec3 v);
s32  vec3_length(Vec3 v);

/* ========================================================================
 * Matrix Operations (all in IWRAM)
 * ======================================================================== */
Mat4 mat4_identity(void);
Mat4 mat4_multiply(Mat4 a, Mat4 b);
Mat4 mat4_translate(s32 x, s32 y, s32 z);
Mat4 mat4_rotate_y(s32 angle);   /* angle: 0-255 = 0-360 degrees */
Mat4 mat4_rotate_x(s32 angle);
Mat4 mat4_scale(s32 sx, s32 sy, s32 sz);

/* ========================================================================
 * Transform Pipeline (IWRAM)
 * ======================================================================== */

/* Transform vertex by 4x4 matrix: result = M * v */
Vec3 vec3_transform(Vec3 v, Mat4 m);

/* Build view matrix from camera position, target, and up vector */
Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up);

/* Build perspective projection matrix (orthographic for GBA) */
Mat4 mat4_ortho(s32 w, s32 h, s32 near_z, s32 far_z);

/* Project 3D point to 2D screen coordinates */
void project_vertex(Vec3 world, s32 screen_w, s32 screen_h, s32 focal_len,
                    ScreenVertex* out);

/* ========================================================================
 * Trigonometry - Precomputed lookup tables (256 entries)
 * ======================================================================== */
void math_init(void);

/* Returns sin(angle) in 8.8 fixed-point, angle: 0-255 */
s32 sin_lut(s32 angle);

/* Returns cos(angle) in 8.8 fixed-point, angle: 0-255 */
s32 cos_lut(s32 angle);

/* Compute face normal from 3 vertices (cross product of edges) */
Vec3 compute_face_normal(Vec3 v0, Vec3 v1, Vec3 v2);

/* Fixed-point absolute value */
static inline s32 fp_abs(s32 v) {
    if (v < 0) return -v;
    return v;
}

/* Fixed-point integer part (floor) */
static inline s32 fp_floor(s32 v) {
    return v >> 8;
}

/* Fixed-point fractional part */
static inline s32 fp_frac(s32 v) {
    return v & 0xFF;
}

#endif /* MATH3D_H */
