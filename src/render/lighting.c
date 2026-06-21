/*
 * Phantom Blade - Lighting System Implementation
 * Fixed-point directional + ambient lighting
 */

#include "lighting.h"
#include "math3d.h"
#include "gba_types.h"

static Lighting light;

void lighting_init(void) {
    /* Sun from upper-right-back */
    light.direction = vec3_normalize(vec3_make(
        INT_TO_F88(-1), INT_TO_F88(1), INT_TO_F88(-1)
    ));
    light.ambient_color = RGB555(4, 4, 6);    /* Dark ambient (dungeon) */
    light.sun_color = RGB555(24, 22, 20);     /* Warm torchlight */
    light.intensity = INT_TO_F88(1);
}

void lighting_set_direction(Vec3 dir) {
    light.direction = vec3_normalize(dir);
}

void lighting_set_ambient(u16 color) {
    light.ambient_color = color;
}

void lighting_set_sun(u16 color, s32 intensity) {
    light.sun_color = color;
    light.intensity = intensity;
}

const Lighting* lighting_get(void) {
    return &light;
}

void lighting_unpack_rgb555(u16 color, s32* r, s32* g, s32* b) {
    *r = color & 0x1F;
    *g = (color >> 5) & 0x1F;
    *b = (color >> 10) & 0x1F;
}

u16 lighting_pack_rgb555(s32 r, s32 g, s32 b) {
    return (u16)((CLAMP(b, 0, 31) << 10) |
                  (CLAMP(g, 0, 31) << 5) |
                  CLAMP(r, 0, 31));
}

/*
 * Compute lit color for a triangle face.
 * Formula: final_color = ambient_color + base_color * max(0, dot(normal, light_dir)) * intensity
 */
u16 lighting_compute_face_color(Vec3 normal, Vec3 light_dir,
                                u16 base_color, u16 ambient) {
    /* Dot product of face normal and light direction (both normalized, 8.8) */
    s32 dot = vec3_dot(normal, light_dir);

    /* Clamp to [0, 1] range (only positive = facing light) */
    if (dot < 0) dot = 0;
    if (dot > INT_TO_F88(1)) dot = INT_TO_F88(1);

    /* Unpack colors */
    s32 br, bg, bb;
    s32 ar, ag, ab;
    lighting_unpack_rgb555(base_color, &br, &bg, &bb);
    lighting_unpack_rgb555(ambient, &ar, &ag, &ab);

    /* Compute diffuse contribution: base * dot */
    s32 dr = (s32)(((s64)br * dot) >> 8);
    s32 dg = (s32)(((s64)bg * dot) >> 8);
    s32 db = (s32)(((s64)bb * dot) >> 8);

    /* Add ambient + diffuse, clamp to [0, 31] */
    s32 fr = ar + dr; if (fr > 31) fr = 31;
    s32 fg = ag + dg; if (fg > 31) fg = 31;
    s32 fb = ab + db; if (fb > 31) fb = 31;

    return lighting_pack_rgb555(fr, fg, fb);
}

/* Convenience wrapper using global lighting */
u16 lighting_apply(Vec3 normal, u16 base_color) {
    return lighting_compute_face_color(normal, light.direction,
                                        base_color, light.ambient_color);
}
