/*
 * Phantom Blade - Camera System Implementation
 * Third-person camera: positioned behind and above the player
 * L/R buttons rotate camera around player
 */

#include "camera.h"
#include "render.h"
#include "math3d.h"
#include "input.h"
#include "gba_types.h"

static Camera cam;

void camera_init(void) {
    cam.yaw = 0;           /* Facing forward (along -Z) */
    cam.pitch = 0;
    cam.distance = INT_TO_F88(12);  /* 12 units behind */
    cam.height = INT_TO_F88(6);     /* 6 units above */
    cam.focal_length = 160;
    cam.position = vec3_make(0, INT_TO_F88(6), INT_TO_F88(-12));
    cam.target = vec3_make(0, INT_TO_F88(2), 0);
    cam.up = vec3_make(0, INT_TO_F88(1), 0);

    render_set_perspective(cam.focal_length);
}

void camera_update(Vec3 target_pos) {
    /* Camera rotation from L/R buttons */
    if (input_key_l()) {
        cam.yaw -= 3;  /* Rotate left */
    }
    if (input_key_r()) {
        cam.yaw += 3;  /* Rotate right */
    }
    cam.yaw &= 0xFF;  /* Wrap to 0-255 */

    /* Compute camera position: orbit around target */
    s32 sin_yaw = sin_lut(cam.yaw);
    s32 cos_yaw = cos_lut(cam.yaw);

    /* distance * sin(yaw), distance * cos(yaw) */
    s32 offset_x = (s32)(((s64)cam.distance * sin_yaw) >> 8);
    s32 offset_z = (s32)(((s64)cam.distance * cos_yaw) >> 8);

    cam.position.x = target_pos.x + offset_x;
    cam.position.y = target_pos.y + cam.height;
    cam.position.z = target_pos.z + offset_z;

    /* Look at a point slightly above player center */
    cam.target.x = target_pos.x;
    cam.target.y = target_pos.y + INT_TO_F88(2);
    cam.target.z = target_pos.z;

    /* Apply camera to render system */
    render_set_camera(cam.position, cam.target, cam.up);
}

void camera_set_yaw(s32 yaw) {
    cam.yaw = yaw & 0xFF;
}

void camera_rotate_yaw(s32 delta) {
    cam.yaw = (cam.yaw + delta) & 0xFF;
}

Vec3 camera_get_position(void) {
    return cam.position;
}

Mat4 camera_get_view_matrix(void) {
    return mat4_look_at(cam.position, cam.target, cam.up);
}

Vec3 camera_get_forward(void) {
    Vec3 fwd = vec3_sub(cam.target, cam.position);
    return vec3_normalize(fwd);
}

Vec3 camera_get_right(void) {
    Vec3 fwd = vec3_get_forward();
    Vec3 right = vec3_cross(fwd, cam.up);
    return vec3_normalize(right);
}
