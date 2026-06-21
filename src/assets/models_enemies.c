/*
 * Phantom Blade - Enemy 3D Models
 * Simple low-poly enemy figures
 */

#include "models_enemies.h"
#include "gba_types.h"
#include "math3d.h"

/* ========================================================================
 * Skeleton Model (~14 triangles)
 * Stick figure with blocky head and ribcage
 * ======================================================================== */
#define COL_BONE      RGB555(24, 22, 20)
#define COL_BONE_DARK RGB555(16, 14, 12)
#define COL_SKULL     RGB555(28, 26, 24)
#define COL_EYE       RGB555(8, 0, 0)

static const Vec3 skeleton_vertices[] = {
    /* Skull */
    { INT_TO_F88(-2), INT_TO_F88(16), INT_TO_F88(-2) },  /* 0 */
    { INT_TO_F88( 2), INT_TO_F88(16), INT_TO_F88(-2) },  /* 1 */
    { INT_TO_F88( 2), INT_TO_F88(16), INT_TO_F88( 2) },  /* 2 */
    { INT_TO_F88(-2), INT_TO_F88(16), INT_TO_F88( 2) },  /* 3 */
    { INT_TO_F88(-2), INT_TO_F88(13), INT_TO_F88(-2) },  /* 4 */
    { INT_TO_F88( 2), INT_TO_F88(13), INT_TO_F88(-2) },  /* 5 */
    { INT_TO_F88( 2), INT_TO_F88(13), INT_TO_F88( 2) },  /* 6 */
    { INT_TO_F88(-2), INT_TO_F88(13), INT_TO_F88( 2) },  /* 7 */
    /* Spine */
    { INT_TO_F88( 0), INT_TO_F88(13), INT_TO_F88( 0) },  /* 8 */
    { INT_TO_F88( 0), INT_TO_F88( 6), INT_TO_F88( 0) },  /* 9 */
    /* Ribcage (wide points) */
    { INT_TO_F88(-2), INT_TO_F88(11), INT_TO_F88( 0) },  /* 10 */
    { INT_TO_F88( 2), INT_TO_F88(11), INT_TO_F88( 0) },  /* 11 */
    /* Arms */
    { INT_TO_F88(-4), INT_TO_F88(10), INT_TO_F88( 0) },  /* 12 */
    { INT_TO_F88( 4), INT_TO_F88(10), INT_TO_F88( 0) },  /* 13 */
    { INT_TO_F88(-3), INT_TO_F88( 5), INT_TO_F88( 0) },  /* 14 */
    { INT_TO_F88( 3), INT_TO_F88( 5), INT_TO_F88( 0) },  /* 15 */
    /* Legs */
    { INT_TO_F88(-1), INT_TO_F88( 6), INT_TO_F88( 0) },  /* 16 */
    { INT_TO_F88( 1), INT_TO_F88( 6), INT_TO_F88( 0) },  /* 17 */
    { INT_TO_F88(-2), INT_TO_F88( 0), INT_TO_F88( 0) },  /* 18 */
    { INT_TO_F88( 2), INT_TO_F88( 0), INT_TO_F88( 0) },  /* 19 */
};

static const Face skeleton_faces[] = {
    /* Skull top */
    { {0, 2, 1}, COL_SKULL },
    { {0, 3, 2}, COL_SKULL },
    /* Skull front */
    { {0, 1, 5}, COL_SKULL },
    { {0, 5, 4}, COL_SKULL },
    /* Skull sides */
    { {0, 4, 7}, COL_SKULL },
    { {1, 2, 6}, COL_SKULL },
    /* Ribcage triangles (flat) */
    { {8, 10, 11}, COL_BONE },
    { {8, 11, 9}, COL_BONE },
    /* Left arm */
    { {10, 12, 14}, COL_BONE },
    { {10, 14, 9}, COL_BONE },
    /* Right arm */
    { {11, 13, 15}, COL_BONE },
    { {11, 15, 9}, COL_BONE },
    /* Left leg */
    { {16, 18, 19}, COL_BONE },
    { {16, 19, 17}, COL_BONE },
    /* Right leg */
    { {16, 18, 9}, COL_BONE },
    { {17, 19, 9}, COL_BONE },
};

const Model model_skeleton_data = {
    .vertices = skeleton_vertices,
    .faces = skeleton_faces,
    .num_vertices = ARRAY_SIZE(skeleton_vertices),
    .num_faces = ARRAY_SIZE(skeleton_faces),
    .bounds_min_x = INT_TO_F88(-4),
    .bounds_max_x = INT_TO_F88(4),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(16),
    .bounds_min_z = INT_TO_F88(-2),
    .bounds_max_z = INT_TO_F88(2),
};

/* ========================================================================
 * Slime Model (~8 triangles)
 * Hemisphere/blob shape sitting on ground
 * ======================================================================== */
#define COL_SLIME     RGB555(4, 24, 12)
#define COL_SLIME_DK  RGB555(2, 16, 8)
#define COL_SLIME_EYE RGB555(24, 24, 4)

static const Vec3 slime_vertices[] = {
    /* Base circle (y=0) */
    { INT_TO_F88(-3), INT_TO_F88( 0), INT_TO_F88(-3) },  /* 0 */
    { INT_TO_F88( 3), INT_TO_F88( 0), INT_TO_F88(-3) },  /* 1 */
    { INT_TO_F88( 3), INT_TO_F88( 0), INT_TO_F88( 3) },  /* 2 */
    { INT_TO_F88(-3), INT_TO_F88( 0), INT_TO_F88( 3) },  /* 3 */
    /* Mid ring (y=2) */
    { INT_TO_F88(-3), INT_TO_F88( 4), INT_TO_F88(-3) },  /* 4 */
    { INT_TO_F88( 3), INT_TO_F88( 4), INT_TO_F88(-3) },  /* 5 */
    { INT_TO_F88( 3), INT_TO_F88( 4), INT_TO_F88( 3) },  /* 6 */
    { INT_TO_F88(-3), INT_TO_F88( 4), INT_TO_F88( 3) },  /* 7 */
    /* Top (y=4) */
    { INT_TO_F88( 0), INT_TO_F88( 6), INT_TO_F88( 0) },  /* 8 */
};

static const Face slime_faces[] = {
    /* Front */
    { {0, 1, 5}, COL_SLIME },
    { {0, 5, 4}, COL_SLIME },
    /* Right */
    { {1, 2, 6}, COL_SLIME_DK },
    { {1, 6, 5}, COL_SLIME_DK },
    /* Back */
    { {2, 3, 7}, COL_SLIME },
    { {2, 7, 6}, COL_SLIME },
    /* Left */
    { {3, 0, 4}, COL_SLIME_DK },
    { {3, 4, 7}, COL_SLIME_DK },
    /* Top */
    { {4, 5, 8}, COL_SLIME },
    { {5, 6, 8}, COL_SLIME },
    { {6, 7, 8}, COL_SLIME },
    { {7, 4, 8}, COL_SLIME },
};

const Model model_slime_data = {
    .vertices = slime_vertices,
    .faces = slime_faces,
    .num_vertices = ARRAY_SIZE(slime_vertices),
    .num_faces = ARRAY_SIZE(slime_faces),
    .bounds_min_x = INT_TO_F88(-3),
    .bounds_max_x = INT_TO_F88(3),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(6),
    .bounds_min_z = INT_TO_F88(-3),
    .bounds_max_z = INT_TO_F88(3),
};

/* ========================================================================
 * Bat Model (~10 triangles)
 * Flat body with wing triangles
 * ======================================================================== */
#define COL_BAT_BODY  RGB555(16, 8, 20)
#define COL_BAT_WING  RGB555(12, 6, 16)
#define COL_BAT_EYE   RGB555(24, 4, 4)

static const Vec3 bat_vertices[] = {
    /* Body center */
    { INT_TO_F88( 0), INT_TO_F88( 0), INT_TO_F88( 0) },  /* 0 */
    /* Body outline */
    { INT_TO_F88(-1), INT_TO_F88( 1), INT_TO_F88(-1) },  /* 1 */
    { INT_TO_F88( 1), INT_TO_F88( 1), INT_TO_F88(-1) },  /* 2 */
    { INT_TO_F88( 1), INT_TO_F88( 1), INT_TO_F88( 1) },  /* 3 */
    { INT_TO_F88(-1), INT_TO_F88( 1), INT_TO_F88( 1) },  /* 4 */
    { INT_TO_F88(-1), INT_TO_F88(-1), INT_TO_F88(-1) },  /* 5 */
    { INT_TO_F88( 1), INT_TO_F88(-1), INT_TO_F88(-1) },  /* 6 */
    { INT_TO_F88( 1), INT_TO_F88(-1), INT_TO_F88( 1) },  /* 7 */
    { INT_TO_F88(-1), INT_TO_F88(-1), INT_TO_F88( 1) },  /* 8 */
    /* Wing tips */
    { INT_TO_F88(-6), INT_TO_F88( 2), INT_TO_F88(-1) },  /* 9 left wing */
    { INT_TO_F88(-5), INT_TO_F88( 0), INT_TO_F88( 0) },  /* 10 left wing mid */
    { INT_TO_F88( 6), INT_TO_F88( 2), INT_TO_F88(-1) },  /* 11 right wing */
    { INT_TO_F88( 5), INT_TO_F88( 0), INT_TO_F88( 0) },  /* 12 right wing mid */
};

static const Face bat_faces[] = {
    /* Body top */
    { {1, 2, 3}, COL_BAT_BODY },
    { {1, 3, 4}, COL_BAT_BODY },
    /* Body bottom */
    { {5, 7, 6}, COL_BAT_BODY },
    { {5, 8, 7}, COL_BAT_BODY },
    /* Body front */
    { {1, 2, 6}, COL_BAT_BODY },
    { {1, 6, 5}, COL_BAT_BODY },
    /* Left wing */
    { {1, 9, 10}, COL_BAT_WING },
    { {1, 10, 4}, COL_BAT_WING },
    /* Right wing */
    { {2, 11, 12}, COL_BAT_WING },
    { {2, 12, 3}, COL_BAT_WING },
};

const Model model_bat_data = {
    .vertices = bat_vertices,
    .faces = bat_faces,
    .num_vertices = ARRAY_SIZE(bat_vertices),
    .num_faces = ARRAY_SIZE(bat_faces),
    .bounds_min_x = INT_TO_F88(-6),
    .bounds_max_x = INT_TO_F88(6),
    .bounds_min_y = INT_TO_F88(-1),
    .bounds_max_y = INT_TO_F88(2),
    .bounds_min_z = INT_TO_F88(-1),
    .bounds_max_z = INT_TO_F88(1),
};
