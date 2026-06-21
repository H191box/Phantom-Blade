/*
 * Phantom Blade - 3D Math Library Implementation
 * All hot functions in IWRAM for maximum GBA performance
 * Uses 8.8 fixed-point arithmetic throughout (s32 base type)
 *
 * Fixed-point conventions:
 *   Vertices: 8.8 format (x256 for integers)
 *   Matrix elements: 8.8 format
 *   Screen coords: pixel.8 sub-pixel precision
 *   Trig values: 8.8 range [-1.0, 1.0] = [-256, 256]
 */

#include "math3d.h"
#include <string.h>

/* ========================================================================
 * Trigonometry Lookup Table
 * 256 entries covering 0-360 degrees
 * Values in 8.8 fixed-point, range [-256, 256] = [-1.0, 1.0]
 * Stored in ROM (const) to save RAM
 * ======================================================================== */
static const s16 sin_table[256] = {
       0,    6,   13,   19,   26,   32,   39,   45,
      52,   58,   65,   71,   78,   84,   90,   97,
     103,  110,  116,  122,  129,  135,  141,  147,
     153,  159,  166,  172,  178,  184,  190,  195,
     201,  207,  213,  218,  224,  229,  235,  240,
     245,  250,  255,  256,  256,  256,  256,  256,
     256,  256,  256,  256,  256,  255,  250,  245,
     240,  235,  229,  224,  218,  213,  207,  201,
     195,  190,  184,  178,  172,  166,  159,  153,
     147,  141,  135,  129,  122,  116,  110,  103,
      97,   90,   84,   78,   71,   65,   58,   52,
      45,   39,   32,   26,   19,   13,    6,    0,
      -6,  -13,  -19,  -26,  -32,  -39,  -45,  -52,
     -58,  -65,  -71,  -78,  -84,  -90,  -97, -103,
    -110, -116, -122, -129, -135, -141, -147, -153,
    -159, -166, -172, -178, -184, -190, -195, -201,
    -207, -213, -218, -224, -229, -235, -240, -245,
    -250, -255, -256, -256, -256, -256, -256, -256,
    -256, -256, -256, -256, -256, -255, -250, -245,
    -240, -235, -229, -224, -218, -213, -207, -201,
    -195, -190, -184, -178, -172, -166, -159, -153,
    -147, -141, -135, -129, -122, -116, -110, -103,
     -97,  -90,  -84,  -78,  -71,  -65,  -58,  -52,
     -45,  -39,  -32,  -26,  -19,  -13,   -6,    0
};

void math_init(void) {
    /* Tables are const (in ROM), nothing to initialize */
}

/* ========================================================================
 * Trigonometry - Table Lookup (IWRAM)
 * ======================================================================== */

s32 sin_lut(s32 angle) {
    /* Mask angle to 0-255 range */
    angle &= 0xFF;
    return (s32)sin_table[angle];
}

s32 cos_lut(s32 angle) {
    /* cos(x) = sin(x + 90 degrees) = sin(x + 64) */
    return sin_lut(angle + 64);
}

/* ========================================================================
 * Vector Operations (IWRAM hot path)
 * ======================================================================== */

Vec3 vec3_make(s32 x, s32 y, s32 z) {
    Vec3 v = {x, y, z};
    return v;
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

Vec3 vec3_scale(Vec3 v, s32 s) {
    Vec3 r;
    /* s is integer scale, v is 8.8 */
    r.x = (v.x * s) >> 8;
    r.y = (v.y * s) >> 8;
    r.z = (v.z * s) >> 8;
    return r;
}

s32 vec3_dot(Vec3 a, Vec3 b) {
    /* Dot product: ax*bx + ay*by + az*bz (all 8.8, result 8.8) */
    s64 dot = (s64)a.x * b.x + (s64)a.y * b.y + (s64)a.z * b.z;
    return (s32)(dot >> 8);
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 r;
    /* Cross product: need 32-bit intermediates for 8.8 inputs */
    s64 cx = (s64)a.y * b.z - (s64)a.z * b.y;
    s64 cy = (s64)a.z * b.x - (s64)a.x * b.z;
    s64 cz = (s64)a.x * b.y - (s64)a.y * b.x;
    r.x = (s32)(cx >> 8);
    r.y = (s32)(cy >> 8);
    r.z = (s32)(cz >> 8);
    return r;
}

Vec3 vec3_normalize(Vec3 v) {
    s32 len_sq = vec3_dot(v, v);  /* length squared in 8.8 */
    if (len_sq == 0) {
        return vec3_make(0, 0, 0);
    }
    /* len_sq is in 8.8 format, but since it's dot(v,v), it's 8.8 * 8.8 >> 8 = 8.8 */
    /* We need sqrt. Use approximate fixed-point sqrt */
    /* Convert to integer-ish range, compute sqrt, convert back */
    s32 int_len_sq = (len_sq + 128) >> 8;  /* approximate integer part */
    if (int_len_sq <= 0) return vec3_make(0, 0, 0);

    /* Newton-Raphson integer square root */
    s32 sq = int_len_sq;
    s32 root = int_len_sq;
    int i;
    for (i = 0; i < 8; i++) {
        if (root == 0) break;
        root = (root + int_len_sq / root) >> 1;
    }

    if (root == 0) return vec3_make(0, 0, 0);

    /* Scale factor = 256 / root (to normalize 8.8 vector) */
    /* This is approximate but good enough for normals */
    s32 inv_len = (256 << 8) / root;  /* 16.8 format result */

    Vec3 r;
    r.x = (s32)(((s64)v.x * inv_len) >> 8);
    r.y = (s32)(((s64)v.y * inv_len) >> 8);
    r.z = (s32)(((s64)v.z * inv_len) >> 8);
    return r;
}

s32 vec3_length(Vec3 v) {
    s32 len_sq = vec3_dot(v, v);
    s32 int_len_sq = (len_sq + 128) >> 8;
    if (int_len_sq <= 0) return 0;
    s32 root = int_len_sq;
    int i;
    for (i = 0; i < 10; i++) {
        if (root == 0) break;
        root = (root + int_len_sq / root) >> 1;
    }
    return root;
}

Vec3 compute_face_normal(Vec3 v0, Vec3 v1, Vec3 v2) {
    Vec3 e1 = vec3_sub(v1, v0);
    Vec3 e2 = vec3_sub(v2, v0);
    Vec3 n = vec3_cross(e1, e2);
    return vec3_normalize(n);
}

/* ========================================================================
 * Matrix Operations (IWRAM hot path)
 * Row-major storage: m[row][col]
 * Transform: result[i] = sum(M[i][j] * v[j]) for j=0..3
 * ======================================================================== */

Mat4 mat4_identity(void) {
    Mat4 m;
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            m.m[i][j] = (i == j) ? INT_TO_F88(1) : 0;
        }
    }
    return m;
}

Mat4 mat4_multiply(Mat4 a, Mat4 b) {
    Mat4 r;
    int i, j, k;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            s64 sum = 0;
            for (k = 0; k < 4; k++) {
                sum += (s64)a.m[i][k] * b.m[k][j];
            }
            r.m[i][j] = (s32)(sum >> 8);
        }
    }
    return r;
}

Mat4 mat4_translate(s32 tx, s32 ty, s32 tz) {
    Mat4 m = mat4_identity();
    /* Translation values go in column 3 (row-major: m[0][3], m[1][3], m[2][3]) */
    m.m[0][3] = tx;  /* already in 8.8 if passed correctly */
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}

Mat4 mat4_rotate_y(s32 angle) {
    Mat4 m = mat4_identity();
    s32 s = sin_lut(angle);
    s32 c = cos_lut(angle);
    /* Rotation around Y axis */
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
}

Mat4 mat4_rotate_x(s32 angle) {
    Mat4 m = mat4_identity();
    s32 s = sin_lut(angle);
    s32 c = cos_lut(angle);
    /* Rotation around X axis */
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}

Mat4 mat4_scale(s32 sx, s32 sy, s32 sz) {
    Mat4 m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
}

/* ========================================================================
 * Vertex Transform Pipeline (IWRAM)
 * Transform a 3D vertex by a 4x4 matrix
 * v = [x, y, z, 1] (homogeneous)
 * ======================================================================== */

Vec3 vec3_transform(Vec3 v, Mat4 m) {
    Vec3 r;
    s64 x = (s64)m.m[0][0] * v.x + (s64)m.m[0][1] * v.y +
            (s64)m.m[0][2] * v.z + (s64)m.m[0][3] * INT_TO_F88(1);
    s64 y = (s64)m.m[1][0] * v.x + (s64)m.m[1][1] * v.y +
            (s64)m.m[1][2] * v.z + (s64)m.m[1][3] * INT_TO_F88(1);
    s64 z = (s64)m.m[2][0] * v.x + (s64)m.m[2][1] * v.y +
            (s64)m.m[2][2] * v.z + (s64)m.m[2][3] * INT_TO_F88(1);

    r.x = (s32)(x >> 8);
    r.y = (s32)(y >> 8);
    r.z = (s32)(z >> 8);
    return r;
}

/* ========================================================================
 * View Matrix (Look-At)
 * Simplified for GBA: computes view rotation + translation
 * ======================================================================== */

Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 forward = vec3_sub(target, eye);
    forward = vec3_normalize(forward);

    Vec3 right = vec3_cross(forward, up);
    right = vec3_normalize(right);

    Vec3 real_up = vec3_cross(right, forward);

    Mat4 m;
    /* View matrix rows (transpose of camera basis vectors) */
    m.m[0][0] = right.x;    m.m[0][1] = right.y;    m.m[0][2] = right.z;
    m.m[1][0] = real_up.x;   m.m[1][1] = real_up.y;   m.m[1][2] = real_up.z;
    m.m[2][0] = -forward.x; m.m[2][1] = -forward.y; m.m[2][2] = -forward.z;
    m.m[3][0] = 0;           m.m[3][1] = 0;           m.m[3][2] = 0;

    /* Translation component: negate dot products */
    m.m[0][3] = -(s32)(((s64)right.x * eye.x + (s64)right.y * eye.y + (s64)right.z * eye.z) >> 8);
    m.m[1][3] = -(s32)(((s64)real_up.x * eye.x + (s64)real_up.y * eye.y + (s64)real_up.z * eye.z) >> 8);
    m.m[2][3] = (s32)(((s64)forward.x * eye.x + (s64)forward.y * eye.y + (s64)forward.z * eye.z) >> 8);
    m.m[3][3] = INT_TO_F88(1);

    return m;
}

/* ========================================================================
 * Projection
 * Simple perspective projection: screen_x = x * focal / z
 * Uses 8.8 fixed-point, focal length in pixels
 * ======================================================================== */

void project_vertex(Vec3 cam_space, s32 screen_w, s32 screen_h,
                    s32 focal_len, ScreenVertex* out) {
    /*
     * cam_space is in 8.8 fixed-point
     * focal_len is integer (pixels, e.g., 128)
     * We need: sx = x * focal / z + cx, sy = y * focal / z + cy
     * With 8.8 inputs:
     *   x * focal = x_8.8 * focal_int → shift by 8 to get result in 8.8
     *   Then divide by z (8.8) → shift by 8 to compensate
     * Total: (x * focal << 8) / z  → result in 8.8
     */
    if (cam_space.z >= INT_TO_F88(1)) {
        /* Only project vertices in front of camera */
        s64 proj_x = ((s64)cam_space.x * (s64)(focal_len << 8)) / cam_space.z;
        s64 proj_y = ((s64)cam_space.y * (s64)(focal_len << 8)) / cam_space.z;

        out->x = (s32)(proj_x >> 8) + (screen_w << 7);  /* + center_x (screen_w/2) */
        out->y = (s32)(proj_y >> 8) + (screen_h << 7);  /* + center_y (screen_h/2) */
        out->z = cam_space.z >> 8;  /* Integer depth for sorting */
    } else {
        out->x = -32768;  /* Off-screen marker */
        out->y = -32768;
        out->z = -32768;
    }
}
