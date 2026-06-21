/*
 * Phantom Blade - Lighting System
 * Simple directional + ambient lighting for flat-shaded triangles
 */

#ifndef LIGHTING_H
#define LIGHTING_H

#include "gba_types.h"
#include "math3d.h"

/* Lighting configuration */
typedef struct {
    Vec3 direction;     /* Direction TO light (normalized, 8.8) */
    u16 ambient_color;  /* Ambient light color (RGB555) */
    u16 sun_color;      /* Direct sunlight color (RGB555) */
    s32 intensity;      /* Sun intensity multiplier (8.8, 0.0-1.0) */
} Lighting;

/* Initialize lighting with defaults */
void lighting_init(void);

/* Set sun direction */
void lighting_set_direction(Vec3 dir);

/* Set ambient and sun colors */
void lighting_set_ambient(u16 color);
void lighting_set_sun(u16 color, s32 intensity);

/* Get current lighting config */
const Lighting* lighting_get(void);

/* Compute lit color for a face given its normal */
u16 lighting_compute_face_color(Vec3 normal, Vec3 light_dir,
                                u16 base_color, u16 ambient);

/* Convenience: compute color using global lighting settings */
u16 lighting_apply(Vec3 normal, u16 base_color);

/* Unpack/pack RGB555 helpers */
void lighting_unpack_rgb555(u16 color, s32* r, s32* g, s32* b);
u16  lighting_pack_rgb555(s32 r, s32 g, s32 b);

#endif /* LIGHTING_H */
