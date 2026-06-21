/*
 * Phantom Blade - 3D Model System
 * Models are arrays of vertices and indexed triangle faces
 * Loaded from const ROM data, transformed and rendered at runtime
 */

#ifndef MODEL_H
#define MODEL_H

#include "gba_types.h"
#include "math3d.h"

/* ========================================================================
 * Triangle Face (index + color)
 * ======================================================================== */
typedef struct {
    u8 v[3];      /* Indices into vertex array (0-255) */
    u16 color;    /* Base RGB555 color (before lighting) */
} Face;

/* ========================================================================
 * 3D Model
 * ======================================================================== */
typedef struct {
    const Vec3* vertices;   /* Pointer to vertex array (in ROM) */
    const Face*  faces;     /* Pointer to face array (in ROM) */
    s32 num_vertices;
    s32 num_faces;
    /* Bounding box (in model space) */
    s32 bounds_min_x, bounds_max_x;
    s32 bounds_min_y, bounds_max_y;
    s32 bounds_min_z, bounds_max_z;
} Model;

/* ========================================================================
 * Model IDs
 * ======================================================================== */
typedef enum {
    MODEL_PLAYER = 0,
    MODEL_SKELETON,
    MODEL_SLIME,
    MODEL_BAT,
    MODEL_WALL,
    MODEL_FLOOR,
    MODEL_CEILING,
    MODEL_DOOR,
    MODEL_PILLAR,
    MODEL_CHEST,
    MODEL_WEAPON_SWORD,
    MODEL_TOTAL_COUNT
} ModelID;

/* ========================================================================
 * Model System API
 * ======================================================================== */

/* Initialize model system (load model pointers) */
void model_init(void);

/* Get a model by ID (returns const Model*) */
const Model* model_get(ModelID id);

/* Render a model at given position and rotation */
void model_render(const Model* mdl, Vec3 pos, s32 rot_y, u16 tint_color);

/* Render a model with flat shading (world-space lighting applied) */
void model_render_lit(const Model* mdl, Vec3 pos, s32 rot_y, Vec3 light_dir,
                      u16 ambient_color);

/* Get model bounding sphere radius */
s32 model_get_radius(const Model* mdl);

#endif /* MODEL_H */
