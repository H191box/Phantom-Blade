/*
 * Phantom Blade - 3D Model System Implementation
 * Transforms and renders indexed triangle models
 */

#include "model.h"
#include "render.h"
#include "lighting.h"
#include "math3d.h"

/* Model table */
static const Model* models[MODEL_TOTAL_COUNT];
static bool initialized = FALSE;

void model_init(void) {
    /* Load model data pointers from assets (defined in models_*.c) */
    extern const Model model_player_data;
    extern const Model model_skeleton_data;
    extern const Model model_slime_data;
    extern const Model model_bat_data;
    extern const Model model_wall_data;
    extern const Model model_floor_data;
    extern const Model model_ceiling_data;
    extern const Model model_door_data;
    extern const Model model_pillar_data;
    extern const Model model_chest_data;
    extern const Model model_sword_data;

    models[MODEL_PLAYER]   = &model_player_data;
    models[MODEL_SKELETON]  = &model_skeleton_data;
    models[MODEL_SLIME]     = &model_slime_data;
    models[MODEL_BAT]       = &model_bat_data;
    models[MODEL_WALL]      = &model_wall_data;
    models[MODEL_FLOOR]     = &model_floor_data;
    models[MODEL_CEILING]   = &model_ceiling_data;
    models[MODEL_DOOR]      = &model_door_data;
    models[MODEL_PILLAR]   = &model_pillar_data;
    models[MODEL_CHEST]    = &model_chest_data;
    models[MODEL_WEAPON_SWORD] = &model_sword_data;

    initialized = TRUE;
}

const Model* model_get(ModelID id) {
    if (!initialized || id >= MODEL_TOTAL_COUNT) return NULL;
    return models[id];
}

/*
 * Render a model at a given position with Y rotation.
 * Uses flat shading with the model's base colors (no lighting).
 */
void model_render(const Model* mdl, Vec3 pos, s32 rot_y, u16 tint_color) {
    if (mdl == NULL) return;

    /* Build model transform: translate to position + rotate Y */
    Mat4 rot = mat4_rotate_y(rot_y);
    Mat4 trans = mat4_translate(pos.x, pos.y, pos.z);
    Mat4 model_mat = mat4_multiply(trans, rot);
    render_set_model_matrix(model_mat);

    /* Submit all faces */
    s32 i;
    for (i = 0; i < mdl->num_faces; i++) {
        const Face* f = &mdl->faces[i];
        Vec3 v0 = mdl->vertices[f->v[0]];
        Vec3 v1 = mdl->vertices[f->v[1]];
        Vec3 v2 = mdl->vertices[f->v[2]];
        u16 color = tint_color != 0 ? tint_color : f->color;
        render_triangle(v0, v1, v2, color);
    }
}

/*
 * Render model with lighting applied.
 * Each face gets directional + ambient lighting.
 */
void model_render_lit(const Model* mdl, Vec3 pos, s32 rot_y,
                      Vec3 light_dir, u16 ambient_color) {
    if (mdl == NULL) return;

    Mat4 rot = mat4_rotate_y(rot_y);
    Mat4 trans = mat4_translate(pos.x, pos.y, pos.z);
    Mat4 model_mat = mat4_multiply(trans, rot);
    render_set_model_matrix(model_mat);

    s32 i;
    for (i = 0; i < mdl->num_faces; i++) {
        const Face* f = &mdl->faces[i];
        Vec3 v0 = mdl->vertices[f->v[0]];
        Vec3 v1 = mdl->vertices[f->v[1]];
        Vec3 v2 = mdl->vertices[f->v[2]];

        /* Compute face normal in model space */
        Vec3 normal = compute_face_normal(v0, v1, v2);

        /* Apply lighting */
        u16 lit_color = lighting_compute_face_color(normal, light_dir,
                                                     f->color, ambient_color);

        render_triangle(v0, v1, v2, lit_color);
    }
}

s32 model_get_radius(const Model* mdl) {
    if (mdl == NULL) return 0;
    s32 dx = mdl->bounds_max_x - mdl->bounds_min_x;
    s32 dy = mdl->bounds_max_y - mdl->bounds_min_y;
    s32 dz = mdl->bounds_max_z - mdl->bounds_min_z;
    return (MAX(dx, MAX(dy, dz)) >> 1);
}
