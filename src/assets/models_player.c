/*
 * Phantom Blade - Player 3D Model
 * Blocky humanoid: head, torso, arms, legs + sword
 * ~24 triangles total
 * All coordinates in 8.8 fixed-point, centered at origin
 * Unit size: 1 unit = 1 block (character is ~2 units tall)
 */

#include "models_player.h"
#include "gba_types.h"
#include "math3d.h"

/* ========================================================================
 * Color Palette
 * ======================================================================== */
#define COL_SKIN      RGB555(24, 18, 14)
#define COL_SHIRT     RGB555(12, 16, 28)
#define COL_PANTS     RGB555(10, 10, 20)
#define COL_BOOTS     RGB555(12, 8, 6)
#define COL_SWORD     RGB555(20, 20, 24)
#define COL_HILT      RGB555(16, 12, 8)
#define COL_HAIR      RGB555(8, 6, 4)

/*
 * Player vertices
 * Character faces -Z direction (forward)
 * Origin at feet, Y-up
 * Total: 2 units wide, 2 units tall, 1 unit deep
 */
static const Vec3 player_vertices[] = {
    /* 0-3: Head (cube, top at y=2, bottom at y=1.6) */
    { INT_TO_F88(-3), INT_TO_F88(16),  INT_TO_F88(-2) },  /* 0 */
    { INT_TO_F88( 3), INT_TO_F88(16),  INT_TO_F88(-2) },  /* 1 */
    { INT_TO_F88( 3), INT_TO_F88(16),  INT_TO_F88( 2) },  /* 2 */
    { INT_TO_F88(-3), INT_TO_F88(16),  INT_TO_F88( 2) },  /* 3 */
    { INT_TO_F88(-3), INT_TO_F88(13),  INT_TO_F88(-2) },  /* 4 */
    { INT_TO_F88( 3), INT_TO_F88(13),  INT_TO_F88(-2) },  /* 5 */
    { INT_TO_F88( 3), INT_TO_F88(13),  INT_TO_F88( 2) },  /* 6 */
    { INT_TO_F88(-3), INT_TO_F88(13),  INT_TO_F88( 2) },  /* 7 */

    /* 8-11: Torso (cube, top at y=1.3, bottom at y=0.6) */
    { INT_TO_F88(-3), INT_TO_F88(13),  INT_TO_F88(-2) },  /* 8 (same as 4) */
    { INT_TO_F88( 3), INT_TO_F88(13),  INT_TO_F88(-2) },  /* 9 (same as 5) */
    { INT_TO_F88( 3), INT_TO_F88(13),  INT_TO_F88( 2) },  /* 10 (same as 6) */
    { INT_TO_F88(-3), INT_TO_F88(13),  INT_TO_F88( 2) },  /* 11 (same as 7) */
    { INT_TO_F88(-3), INT_TO_F88( 6),   INT_TO_F88(-2) },  /* 12 */
    { INT_TO_F88( 3), INT_TO_F88( 6),   INT_TO_F88(-2) },  /* 13 */
    { INT_TO_F88( 3), INT_TO_F88( 6),   INT_TO_F88( 2) },  /* 14 */
    { INT_TO_F88(-3), INT_TO_F88( 6),   INT_TO_F88( 2) },  /* 15 */

    /* 16-19: Right arm (thin box, hanging at side) */
    { INT_TO_F88(-5), INT_TO_F88(13),  INT_TO_F88(-1) },  /* 16 */
    { INT_TO_F88(-4), INT_TO_F88(13),  INT_TO_F88(-1) },  /* 17 */
    { INT_TO_F88(-4), INT_TO_F88(13),  INT_TO_F88( 1) },  /* 18 */
    { INT_TO_F88(-5), INT_TO_F88(13),  INT_TO_F88( 1) },  /* 19 */
    { INT_TO_F88(-5), INT_TO_F88( 7),  INT_TO_F88(-1) },  /* 20 */
    { INT_TO_F88(-4), INT_TO_F88( 7),  INT_TO_F88(-1) },  /* 21 */
    { INT_TO_F88(-4), INT_TO_F88( 7),  INT_TO_F88( 1) },  /* 22 */
    { INT_TO_F88(-5), INT_TO_F88( 7),  INT_TO_F88( 1) },  /* 23 */

    /* 24-27: Left arm (holds sword, extended forward) */
    { INT_TO_F88( 4), INT_TO_F88(13),  INT_TO_F88(-1) },  /* 24 */
    { INT_TO_F88( 5), INT_TO_F88(13),  INT_TO_F88(-1) },  /* 25 */
    { INT_TO_F88( 5), INT_TO_F88(13),  INT_TO_F88( 1) },  /* 26 */
    { INT_TO_F88( 4), INT_TO_F88(13),  INT_TO_F88( 1) },  /* 27 */
    { INT_TO_F88( 4), INT_TO_F88(10),  INT_TO_F88(-4) },  /* 28 (extended) */
    { INT_TO_F88( 5), INT_TO_F88(10),  INT_TO_F88(-4) },  /* 29 */
    { INT_TO_F88( 5), INT_TO_F88(10),  INT_TO_F88(-2) },  /* 30 */
    { INT_TO_F88( 4), INT_TO_F88(10),  INT_TO_F88(-2) },  /* 31 */

    /* 32-35: Right leg */
    { INT_TO_F88(-2), INT_TO_F88( 6),  INT_TO_F88(-1) },  /* 32 */
    { INT_TO_F88( 0), INT_TO_F88( 6),  INT_TO_F88(-1) },  /* 33 */
    { INT_TO_F88( 0), INT_TO_F88( 6),  INT_TO_F88( 1) },  /* 34 */
    { INT_TO_F88(-2), INT_TO_F88( 6),  INT_TO_F88( 1) },  /* 35 */
    { INT_TO_F88(-2), INT_TO_F88( 0),  INT_TO_F88(-1) },  /* 36 */
    { INT_TO_F88( 0), INT_TO_F88( 0),  INT_TO_F88(-1) },  /* 37 */
    { INT_TO_F88( 0), INT_TO_F88( 0),  INT_TO_F88( 1) },  /* 38 */
    { INT_TO_F88(-2), INT_TO_F88( 0),  INT_TO_F88( 1) },  /* 39 */

    /* 40-43: Left leg */
    { INT_TO_F88( 0), INT_TO_F88( 6),  INT_TO_F88(-1) },  /* 40 */
    { INT_TO_F88( 2), INT_TO_F88( 6),  INT_TO_F88(-1) },  /* 41 */
    { INT_TO_F88( 2), INT_TO_F88( 6),  INT_TO_F88( 1) },  /* 42 */
    { INT_TO_F88( 0), INT_TO_F88( 6),  INT_TO_F88( 1) },  /* 43 */
    { INT_TO_F88( 0), INT_TO_F88( 0),  INT_TO_F88(-1) },  /* 44 */
    { INT_TO_F88( 2), INT_TO_F88( 0),  INT_TO_F88(-1) },  /* 45 */
    { INT_TO_F88( 2), INT_TO_F88( 0),  INT_TO_F88( 1) },  /* 46 */
    { INT_TO_F88( 0), INT_TO_F88( 0),  INT_TO_F88( 1) },  /* 47 */

    /* 48-50: Sword blade tip */
    { INT_TO_F88( 5), INT_TO_F88(10),  INT_TO_F88(-8) },  /* 48 */
    { INT_TO_F88( 4), INT_TO_F88(10),  INT_TO_F88(-8) },  /* 49 */
};

/*
 * Player faces (triangle indices + colors)
 */
static const Face player_faces[] = {
    /* Head - top */
    { {0, 2, 1}, COL_HAIR },
    { {0, 3, 2}, COL_HAIR },
    /* Head - front (-Z) */
    { {0, 1, 5}, COL_SKIN },
    { {0, 5, 4}, COL_SKIN },
    /* Head - back (+Z) */
    { {2, 3, 7}, COL_HAIR },
    { {2, 7, 6}, COL_HAIR },
    /* Head - left (-X) */
    { {0, 4, 7}, COL_SKIN },
    { {0, 7, 3}, COL_SKIN },
    /* Head - right (+X) */
    { {1, 2, 6}, COL_SKIN },
    { {1, 6, 5}, COL_SKIN },

    /* Torso - front (-Z) */
    { {8, 9, 13}, COL_SHIRT },
    { {8, 13, 12}, COL_SHIRT },
    /* Torso - back (+Z) */
    { {10, 11, 15}, COL_SHIRT },
    { {10, 15, 14}, COL_SHIRT },
    /* Torso - left (-X) */
    { {8, 12, 15}, COL_SHIRT },
    { {8, 15, 11}, COL_SHIRT },
    /* Torso - right (+X) */
    { {9, 10, 14}, COL_SHIRT },
    { {9, 14, 13}, COL_SHIRT },

    /* Right arm - front */
    { {16, 17, 21}, COL_SHIRT },
    { {16, 21, 20}, COL_SHIRT },
    /* Left arm - front (extended) */
    { {24, 25, 29}, COL_SHIRT },
    { {24, 29, 28}, COL_SHIRT },

    /* Right leg - front */
    { {32, 33, 37}, COL_PANTS },
    { {32, 37, 36}, COL_PANTS },
    /* Right leg - bottom (boots) */
    { {36, 37, 38}, COL_BOOTS },
    { {36, 38, 39}, COL_BOOTS },
    /* Left leg - front */
    { {40, 41, 45}, COL_PANTS },
    { {40, 45, 44}, COL_PANTS },
    /* Left leg - bottom */
    { {44, 45, 46}, COL_BOOTS },
    { {44, 46, 47}, COL_BOOTS },

    /* Sword blade (front face only) */
    { {29, 48, 49}, COL_SWORD },
    { {29, 49, 28}, COL_SWORD },
};

const Model model_player_data = {
    .vertices = player_vertices,
    .faces = player_faces,
    .num_vertices = ARRAY_SIZE(player_vertices),
    .num_faces = ARRAY_SIZE(player_faces),
    .bounds_min_x = INT_TO_F88(-5),
    .bounds_max_x = INT_TO_F88(5),
    .bounds_min_y = INT_TO_F88(0),
    .bounds_max_y = INT_TO_F88(16),
    .bounds_min_z = INT_TO_F88(-8),
    .bounds_max_z = INT_TO_F88(2),
};
