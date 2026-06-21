/*
 * Phantom Blade - Camera System
 * Third-person camera that follows the player
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "gba_types.h"
#include "math3d.h"

/* Camera configuration */
typedef struct {
    Vec3 position;      /* Camera world position */
    Vec3 target;        /* Look-at target */
    Vec3 up;            /* Up vector */
    s32 yaw;            /* Horizontal rotation (0-255) */
    s32 pitch;          /* Vertical angle */
    s32 distance;       /* Distance from target (8.8 fixed) */
    s32 height;         /* Height offset above target */
    s32 focal_length;   /* Perspective focal length (pixels) */
} Camera;

/* Initialize camera with default settings */
void camera_init(void);

/* Update camera to follow a target position */
void camera_update(Vec3 target_pos);

/* Set camera Y rotation directly */
void camera_set_yaw(s32 yaw);

/* Rotate camera yaw by delta */
void camera_rotate_yaw(s32 delta);

/* Get camera position */
Vec3 camera_get_position(void);

/* Get view matrix (for rendering) */
Mat4 camera_get_view_matrix(void);

/* Get camera forward direction */
Vec3 camera_get_forward(void);

/* Get camera right direction */
Vec3 camera_get_right(void);

#endif /* CAMERA_H */
