/*
 * Phantom Blade - World Environment Models
 * Simple geometry pieces for building dungeon rooms
 */

#include "models_world.h"
#include "gba_types.h"
#include "math3d.h"

#define COL_STONE     RGB555(14, 12, 16)
#define COL_STONE_DK  RGB558(10, 8, 12)
#define COL_WOOD      RGB555(20, 16, 8)
#define COL_WOOD_DK   RGB555(14, 10, 6)
#define COL_METAL     RGB555(20, 20, 24)
#define COL_GOLD      RGB555(24, 20, 8)
#define COL_CHEST_LID RGB555(18, 12, 6)

/* Fix typo - RGB558 should be RGB555 */
#undef COL_STONE_DK
#define COL_STONE_DK  RGB555(10, 8, 12)

/* ========================================================================
 * Wall Segment - 2 triangles (single quad)
 * Unit size: 4x8x1
 * ======================================================================== */
static const Vec3 wall_vertices[] = {
    { INT_TO_F88(-2), INT_TO_F88(0),  INT_TO_F88(0) },  /* 0 */
    { INT_TO_F88( 2), INT_TO_F88(0),  INT_TO_F88(0) },  /* 1 */
    { INT_TO_F88( 2), INT_TO_F88(8),  INT_TO_F88(0) },  /* 2 */
    { INT_TO_F88(-2), INT_TO_F88(8),  INT_TO_F88(0) },  /* 3 */
};

static const Face wall_faces[] = {
    { {0, 1, 2}, COL_STONE },
    { {0, 2, 3}, COL_STONE },
};

const Model model_wall_data = {
    .vertices = wall_vertices,
    .faces = wall_faces,
    .num_vertices = ARRAY_SIZE(wall_vertices),
    .num_faces = ARRAY_SIZE(wall_faces),
    .bounds_min_x = INT_TO_F88(-2),
    .bounds_max_x = INT_TO_F88(2),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(8),
    .bounds_min_z = INT_TO_F88(0),
    .bounds_max_z = INT_TO_F88(0),
};

/* ========================================================================
 * Floor Tile - 2 triangles (horizontal quad)
 * ======================================================================== */
static const Vec3 floor_vertices[] = {
    { INT_TO_F88(-2), INT_TO_F88(0),  INT_TO_F88(-2) },  /* 0 */
    { INT_TO_F88( 2), INT_TO_F88(0),  INT_TO_F88(-2) },  /* 1 */
    { INT_TO_F88( 2), INT_TO_F88(0),  INT_TO_F88( 2) },  /* 2 */
    { INT_TO_F88(-2), INT_TO_F88(0),  INT_TO_F88( 2) },  /* 3 */
};

static const Face floor_faces[] = {
    { {0, 1, 2}, RGB555(10, 10, 12) },
    { {0, 2, 3}, RGB555(10, 10, 12) },
};

const Model model_floor_data = {
    .vertices = floor_vertices,
    .faces = floor_faces,
    .num_vertices = ARRAY_SIZE(floor_vertices),
    .num_faces = ARRAY_SIZE(floor_faces),
    .bounds_min_x = INT_TO_F88(-2),
    .bounds_max_x = INT_TO_F88(2),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(0),
    .bounds_min_z = INT_TO_F88(-2),
    .bounds_max_z = INT_TO_F88(2),
};

/* ========================================================================
 * Ceiling Tile - 2 triangles
 * ======================================================================== */
static const Vec3 ceiling_vertices[] = {
    { INT_TO_F88(-2), INT_TO_F88(8),  INT_TO_F88(-2) },  /* 0 */
    { INT_TO_F88( 2), INT_TO_F88(8),  INT_TO_F88(-2) },  /* 1 */
    { INT_TO_F88( 2), INT_TO_F88(8),  INT_TO_F88( 2) },  /* 2 */
    { INT_TO_F88(-2), INT_TO_F88(8),  INT_TO_F88( 2) },  /* 3 */
};

static const Face ceiling_faces[] = {
    { {0, 2, 1}, RGB555(6, 6, 8) },
    { {0, 3, 2}, RGB555(6, 6, 8) },
};

const Model model_ceiling_data = {
    .vertices = ceiling_vertices,
    .faces = ceiling_faces,
    .num_vertices = ARRAY_SIZE(ceiling_vertices),
    .num_faces = ARRAY_SIZE(ceiling_faces),
    .bounds_min_x = INT_TO_F88(-2),
    .bounds_max_x = INT_TO_F88(2),
    .bounds_min_y = INT_TO_F88(8),
    .bounds_max_y = INT_TO_F88(8),
    .bounds_min_z = INT_TO_F88(-2),
    .bounds_max_z = INT_TO_F88(2),
};

/* ========================================================================
 * Door - 2 triangles (wooden quad with frame)
 * ======================================================================== */
static const Vec3 door_vertices[] = {
    { INT_TO_F88(-2), INT_TO_F88(0), INT_TO_F88(0) },  /* 0 */
    { INT_TO_F88( 2), INT_TO_F88(0), INT_TO_F88(0) },  /* 1 */
    { INT_TO_F88( 2), INT_TO_F88(7), INT_TO_F88(0) },  /* 2 */
    { INT_TO_F88(-2), INT_TO_F88(7), INT_TO_F88(0) },  /* 3 */
};

static const Face door_faces[] = {
    { {0, 1, 2}, COL_WOOD },
    { {0, 2, 3}, COL_WOOD_DK },
};

const Model model_door_data = {
    .vertices = door_vertices,
    .faces = door_faces,
    .num_vertices = ARRAY_SIZE(door_vertices),
    .num_faces = ARRAY_SIZE(door_faces),
    .bounds_min_x = INT_TO_F88(-2),
    .bounds_max_x = INT_TO_F88(2),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(7),
    .bounds_min_z = INT_TO_F88(0),
    .bounds_max_z = INT_TO_F88(0),
};

/* ========================================================================
 * Pillar - Box shape, ~10 triangles
 * ======================================================================== */
static const Vec3 pillar_vertices[] = {
    { INT_TO_F88(-1), INT_TO_F88(0), INT_TO_F88(-1) },  /* 0 */
    { INT_TO_F88( 1), INT_TO_F88(0), INT_TO_F88(-1) },  /* 1 */
    { INT_TO_F88( 1), INT_TO_F88(0), INT_TO_F88( 1) },  /* 2 */
    { INT_TO_F88(-1), INT_TO_F88(0), INT_TO_F88( 1) },  /* 3 */
    { INT_TO_F88(-1), INT_TO_F88(9), INT_TO_F88(-1) },  /* 4 */
    { INT_TO_F88( 1), INT_TO_F88(9), INT_TO_F88(-1) },  /* 5 */
    { INT_TO_F88( 1), INT_TO_F88(9), INT_TO_F88( 1) },  /* 6 */
    { INT_TO_F88(-1), INT_TO_F88(9), INT_TO_F88( 1) },  /* 7 */
};

static const Face pillar_faces[] = {
    /* Front */
    { {0, 1, 5}, COL_STONE },
    { {0, 5, 4}, COL_STONE },
    /* Right */
    { {1, 2, 6}, COL_STONE_DK },
    { {1, 6, 5}, COL_STONE_DK },
    /* Back */
    { {2, 3, 7}, COL_STONE },
    { {2, 7, 6}, COL_STONE },
    /* Left */
    { {3, 0, 4}, COL_STONE_DK },
    { {3, 4, 7}, COL_STONE_DK },
    /* Top */
    { {4, 5, 6}, COL_STONE },
    { {4, 6, 7}, COL_STONE },
};

const Model model_pillar_data = {
    .vertices = pillar_vertices,
    .faces = pillar_faces,
    .num_vertices = ARRAY_SIZE(pillar_vertices),
    .num_faces = ARRAY_SIZE(pillar_faces),
    .bounds_min_x = INT_TO_F88(-1),
    .bounds_max_x = INT_TO_F88(1),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(9),
    .bounds_min_z = INT_TO_F88(-1),
    .bounds_max_z = INT_TO_F88(1),
};

/* ========================================================================
 * Treasure Chest - Box with colored lid, ~10 triangles
 * ======================================================================== */
static const Vec3 chest_vertices[] = {
    /* Base */
    { INT_TO_F88(-2), INT_TO_F88(0), INT_TO_F88(-1) },  /* 0 */
    { INT_TO_F88( 2), INT_TO_F88(0), INT_TO_F88(-1) },  /* 1 */
    { INT_TO_F88( 2), INT_TO_F88(0), INT_TO_F88( 1) },  /* 2 */
    { INT_TO_F88(-2), INT_TO_F88(0), INT_TO_F88( 1) },  /* 3 */
    { INT_TO_F88(-2), INT_TO_F88(3), INT_TO_F88(-1) },  /* 4 */
    { INT_TO_F88( 2), INT_TO_F88(3), INT_TO_F88(-1) },  /* 5 */
    { INT_TO_F88( 2), INT_TO_F88(3), INT_TO_F88( 1) },  /* 6 */
    { INT_TO_F88(-2), INT_TO_F88(3), INT_TO_F88( 1) },  /* 7 */
    /* Lid top */
    { INT_TO_F88(-2), INT_TO_F88(3), INT_TO_F88(-1) },  /* 8 = 4 */
    { INT_TO_F88( 2), INT_TO_F88(3), INT_TO_F88(-1) },  /* 9 = 5 */
    { INT_TO_F88( 2), INT_TO_F88(5), INT_TO_F88( 0) },  /* 10 */
    { INT_TO_F88(-2), INT_TO_F88(5), INT_TO_F88( 0) },  /* 11 */
};

static const Face chest_faces[] = {
    /* Base front */
    { {0, 1, 5}, COL_WOOD },
    { {0, 5, 4}, COL_WOOD },
    /* Base right */
    { {1, 2, 6}, COL_WOOD_DK },
    { {1, 6, 5}, COL_WOOD_DK },
    /* Base left */
    { {3, 0, 4}, COL_WOOD_DK },
    { {3, 4, 7}, COL_WOOD_DK },
    /* Base back */
    { {2, 3, 7}, COL_WOOD },
    { {2, 7, 6}, COL_WOOD },
    /* Lid front */
    { {4, 5, 10}, COL_CHEST_LID },
    { {4, 10, 11}, COL_CHEST_LID },
    /* Lid top */
    { {10, 11, 4}, COL_GOLD },
    { {11, 10, 5}, COL_GOLD },
};

const Model model_chest_data = {
    .vertices = chest_vertices,
    .faces = chest_faces,
    .num_vertices = ARRAY_SIZE(chest_vertices),
    .num_faces = ARRAY_SIZE(chest_faces),
    .bounds_min_x = INT_TO_F88(-2),
    .bounds_max_x = INT_TO_F88(2),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(5),
    .bounds_min_z = INT_TO_F88(-1),
    .bounds_max_z = INT_TO_F88(1),
};

/* ========================================================================
 * Sword (standalone pickup) - Simple blade + hilt
 * ======================================================================== */
static const Vec3 sword_vertices[] = {
    /* Blade */
    { INT_TO_F88( 0), INT_TO_F88(10), INT_TO_F88(-1) },  /* 0 tip */
    { INT_TO_F88( 0), INT_TO_F88(10), INT_TO_F88( 1) },  /* 1 tip */
    { INT_TO_F88( 0), INT_TO_F88( 4), INT_TO_F88( 1) },  /* 2 base */
    { INT_TO_F88( 0), INT_TO_F88( 4), INT_TO_F88(-1) },  /* 3 base */
    /* Guard */
    { INT_TO_F88(-2), INT_TO_F88( 4), INT_TO_F88( 0) },  /* 4 */
    { INT_TO_F88( 2), INT_TO_F88( 4), INT_TO_F88( 0) },  /* 5 */
    /* Handle */
    { INT_TO_F88( 0), INT_TO_F88( 2), INT_TO_F88(-1) },  /* 6 */
    { INT_TO_F88( 0), INT_TO_F88( 2), INT_TO_F88( 1) },  /* 7 */
    { INT_TO_F88( 0), INT_TO_F88( 0), INT_TO_F88( 1) },  /* 8 */
    { INT_TO_F88( 0), INT_TO_F88( 0), INT_TO_F88(-1) },  /* 9 */
};

static const Face sword_faces[] = {
    /* Blade front */
    { {0, 2, 1}, COL_METAL },
    { {0, 3, 2}, COL_METAL },
    /* Blade back */
    { {0, 1, 2}, COL_STONE_DK },
    { {0, 2, 3}, COL_STONE_DK },
    /* Guard */
    { {4, 5, 2}, COL_GOLD },
    { {4, 2, 3}, COL_GOLD },
    /* Handle */
    { {6, 7, 8}, COL_WOOD },
    { {6, 8, 9}, COL_WOOD },
};

const Model model_sword_data = {
    .vertices = sword_vertices,
    .faces = sword_faces,
    .num_vertices = ARRAY_SIZE(sword_vertices),
    .num_faces = ARRAY_SIZE(sword_faces),
    .bounds_min_x = INT_TO_F88(-2),
    .bounds_max_x = INT_TO_F88(2),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(10),
    .bounds_min_z = INT_TO_F88(-1),
    .bounds_max_z = INT_TO_F88(1),
};
