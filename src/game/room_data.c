/*
 * Phantom Blade - Room Data Implementation
 * Pre-defined room layouts with 3D geometry
 *
 * Room 0: "Entrance Hall" - simple rectangular room
 * Room 1: "Corridor" - L-shaped corridor
 * Room 2: "Treasure Chamber" - wider room with pillar obstacles
 */

#include "room_data.h"
#include "render.h"
#include "lighting.h"
#include "model.h"
#include "math3d.h"
#include "gba_header.h"

/*
 * Color palette for room geometry
 */
#define COL_FLOOR       RGB555(10, 10, 12)    /* Dark stone floor */
#define COL_CEILING     RGB555(6, 6, 8)       /* Dark ceiling */
#define COL_WALL_NORTH  RGB555(14, 12, 16)   /* North wall */
#define COL_WALL_SOUTH  RGB555(12, 12, 16)
#define COL_WALL_EAST   RGB555(16, 14, 18)
#define COL_WALL_WEST   RGB555(12, 10, 14)
#define COL_DOOR        RGB555(20, 16, 8)     /* Wooden door */
#define COL_PILLAR      RGB555(18, 16, 20)    /* Stone pillar */

/*
 * Room 0: "Entrance Hall"
 * Simple 20×20×8 rectangular room, 1 enemy, south door
 */
static const Vec3 room0_verts[] = {
    /* Floor: 4 corners at y=0 */
    { INT_TO_F88(-10), INT_TO_F88(0),  INT_TO_F88(-10) },  /* 0: floor SW */
    { INT_TO_F88( 10), INT_TO_F88(0),  INT_TO_F88(-10) },  /* 1: floor SE */
    { INT_TO_F88( 10), INT_TO_F88(0),  INT_TO_F88( 10) },  /* 2: floor NE */
    { INT_TO_F88(-10), INT_TO_F88(0),  INT_TO_F88( 10) },  /* 3: floor NW */
    /* Ceiling: 4 corners at y=8 */
    { INT_TO_F88(-10), INT_TO_F88(8),  INT_TO_F88(-10) },  /* 4: ceil SW */
    { INT_TO_F88( 10), INT_TO_F88(8),  INT_TO_F88(-10) },  /* 5: ceil SE */
    { INT_TO_F88( 10), INT_TO_F88(8),  INT_TO_F88( 10) },  /* 6: ceil NE */
    { INT_TO_F88(-10), INT_TO_F88(8),  INT_TO_F88( 10) },  /* 7: ceil NW */
    /* Walls reuse floor/ceiling Y coords at boundaries */
    /* 8-15: wall vertices (already covered by floor/ceiling) */
};

static const Face room0_faces[] = {
    /* Floor (2 triangles) */
    { {0, 1, 2}, COL_FLOOR },
    { {0, 2, 3}, COL_FLOOR },
    /* Ceiling (2 triangles) */
    { {4, 6, 5}, COL_CEILING },
    { {4, 7, 6}, COL_CEILING },
    /* South wall (z=10) */
    { {3, 2, 7}, COL_WALL_SOUTH },
    { {7, 2, 6}, COL_WALL_SOUTH },
    /* North wall (z=-10) */
    { {0, 4, 1}, COL_WALL_NORTH },
    { {1, 4, 5}, COL_WALL_NORTH },
    /* East wall (x=10) */
    { {1, 5, 2}, COL_WALL_EAST },
    { {2, 5, 6}, COL_WALL_EAST },
    /* West wall (x=-10) */
    { {0, 3, 4}, COL_WALL_WEST },
    { {4, 3, 7}, COL_WALL_WEST },
};

static Room room0 = {
    .id = 0,
    .name = "Entrance Hall",
    .width = 20, .depth = 20, .height = 8,
    .num_wall_faces = 8,
    .num_floor_faces = 2,
    .doors = {
        { 1, {INT_TO_F88(0), INT_TO_F88(2), INT_TO_F88(6)}, 0 }
    },
    .num_doors = 1,
    .enemy_spawns = { ENEMY_SKELETON },
    .enemy_positions = {
        { INT_TO_F88(0), INT_TO_F88(2), INT_TO_F88(-3) }
    },
    .num_enemies = 1,
    .item_types = { 1 },  /* Health potion */
    .item_positions = {
        { INT_TO_F88(-3), INT_TO_F88(1), INT_TO_F88(3) }
    },
    .num_items = 1,
    .door_locked = TRUE,
};

/*
 * Room 1: "Corridor"
 * L-shaped corridor: main section + side branch
 */
static const Vec3 room1_verts[] = {
    /* Main corridor (long section) */
    { INT_TO_F88(-3), INT_TO_F88(0),  INT_TO_F88(-12) },  /* 0 */
    { INT_TO_F88( 3), INT_TO_F88(0),  INT_TO_F88(-12) },  /* 1 */
    { INT_TO_F88( 3), INT_TO_F88(0),  INT_TO_F88( 4) },   /* 2 */
    { INT_TO_F88(-3), INT_TO_F88(0),  INT_TO_F88( 4) },   /* 3 */
    /* Side branch */
    { INT_TO_F88( 3), INT_TO_F88(0),  INT_TO_F88(-2) },   /* 4 */
    { INT_TO_F88( 9), INT_TO_F88(0),  INT_TO_F88(-2) },   /* 5 */
    { INT_TO_F88( 9), INT_TO_F88(0),  INT_TO_F88( 4) },   /* 6 */
    /* Ceiling */
    { INT_TO_F88(-3), INT_TO_F88(7),  INT_TO_F88(-12) },  /* 7 */
    { INT_TO_F88( 3), INT_TO_F88(7),  INT_TO_F88(-12) },  /* 8 */
    { INT_TO_F88( 3), INT_TO_F88(7),  INT_TO_F88( 4) },   /* 9 */
    { INT_TO_F88(-3), INT_TO_F88(7),  INT_TO_F88( 4) },   /* 10 */
    { INT_TO_F88( 9), INT_TO_F88(7),  INT_TO_F88( 4) },   /* 11 */
    { INT_TO_F88( 9), INT_TO_F88(7),  INT_TO_F88(-2) },   /* 12 */
    { INT_TO_F88( 3), INT_TO_F88(7),  INT_TO_F88(-2) },   /* 13 */
};

static const Face room1_faces[] = {
    /* Main floor */
    { {0, 1, 2}, COL_FLOOR },
    { {0, 2, 3}, COL_FLOOR },
    /* Side branch floor */
    { {2, 4, 5}, COL_FLOOR },
    { {2, 5, 6}, COL_FLOOR },
    /* Main ceiling */
    { {7, 9, 8}, COL_CEILING },
    { {7, 10, 9}, COL_CEILING },
    /* Side branch ceiling */
    { {9, 11, 12}, COL_CEILING },
    { {9, 12, 13}, COL_CEILING },
    /* South end wall */
    { {3, 2, 10}, COL_WALL_SOUTH },
    { {10, 2, 9}, COL_WALL_SOUTH },
    /* North end wall */
    { {0, 7, 1}, COL_WALL_NORTH },
    { {1, 7, 8}, COL_WALL_NORTH },
    /* Main west wall */
    { {0, 3, 7}, COL_WALL_WEST },
    { {7, 3, 10}, COL_WALL_WEST },
    /* Main east wall (partial - has side branch opening) */
    { {1, 8, 4}, COL_WALL_EAST }, /* below branch */
    { {4, 8, 13}, COL_WALL_EAST },
    /* Side branch south wall */
    { {6, 2, 11}, COL_WALL_SOUTH },
    { {11, 2, 9}, COL_WALL_SOUTH },
    /* Side branch east wall */
    { {5, 12, 6}, COL_WALL_EAST },
    { {6, 12, 11}, COL_WALL_EAST },
    /* Side branch north wall */
    { {4, 13, 5}, COL_WALL_NORTH },
    { {5, 13, 12}, COL_WALL_NORTH },
};

static Room room1 = {
    .id = 1,
    .name = "Corridor",
    .width = 12, .depth = 16, .height = 7,
    .num_wall_faces = 12,
    .num_floor_faces = 2,
    .doors = {
        { 2, {INT_TO_F88(0), INT_TO_F88(2), INT_TO_F88(10)}, 0 },
        { 0, {INT_TO_F88(0), INT_TO_F88(2), INT_TO_F88(-8)}, 0 }
    },
    .num_doors = 2,
    .enemy_spawns = { ENEMY_SLIME, ENEMY_SKELETON },
    .enemy_positions = {
        { INT_TO_F88(0), INT_TO_F88(2), INT_TO_F88(0) },
        { INT_TO_F88(6), INT_TO_F88(2), INT_TO_F88(2) }
    },
    .num_enemies = 2,
    .item_types = { 0, 1 },
    .item_positions = {
        { INT_TO_F88(0), INT_TO_F88(1), INT_TO_F88(-8) },
        { INT_TO_F88(6), INT_TO_F88(1), INT_TO_F88(2) }
    },
    .num_items = 2,
    .door_locked = TRUE,
};

/*
 * Room 2: "Treasure Chamber"
 * Wider room with central pillars and treasure chest
 */
static const Vec3 room2_verts[] = {
    /* Floor */
    { INT_TO_F88(-12), INT_TO_F88(0), INT_TO_F88(-10) },  /* 0 */
    { INT_TO_F88( 12), INT_TO_F88(0), INT_TO_F88(-10) },  /* 1 */
    { INT_TO_F88( 12), INT_TO_F88(0), INT_TO_F88( 10) },  /* 2 */
    { INT_TO_F88(-12), INT_TO_F88(0), INT_TO_F88( 10) },  /* 3 */
    /* Ceiling */
    { INT_TO_F88(-12), INT_TO_F88(9), INT_TO_F88(-10) },  /* 4 */
    { INT_TO_F88( 12), INT_TO_F88(9), INT_TO_F88(-10) },  /* 5 */
    { INT_TO_F88( 12), INT_TO_F88(9), INT_TO_F88( 10) },  /* 6 */
    { INT_TO_F88(-12), INT_TO_F88(9), INT_TO_F88( 10) },  /* 7 */
};

static const Face room2_faces[] = {
    /* Floor */
    { {0, 1, 2}, COL_FLOOR },
    { {0, 2, 3}, COL_FLOOR },
    /* Ceiling */
    { {4, 6, 5}, COL_CEILING },
    { {4, 7, 6}, COL_CEILING },
    /* North wall */
    { {0, 4, 1}, COL_WALL_NORTH },
    { {1, 4, 5}, COL_WALL_NORTH },
    /* South wall */
    { {3, 2, 7}, COL_WALL_SOUTH },
    { {7, 2, 6}, COL_WALL_SOUTH },
    /* East wall */
    { {1, 5, 2}, COL_WALL_EAST },
    { {2, 5, 6}, COL_WALL_EAST },
    /* West wall */
    { {0, 3, 4}, COL_WALL_WEST },
    { {4, 3, 7}, COL_WALL_WEST },
};

static Room room2 = {
    .id = 2,
    .name = "Treasure Chamber",
    .width = 24, .depth = 20, .height = 9,
    .num_wall_faces = 8,
    .num_floor_faces = 2,
    .doors = {
        { 1, {INT_TO_F88(0), INT_TO_F88(2), INT_TO_F88(-6)}, 0 },
        { 0, {INT_TO_F88(0), INT_TO_F88(2), INT_TO_F88(6)}, 128 }
    },
    .num_doors = 2,
    .enemy_spawns = { ENEMY_SKELETON, ENEMY_SLIME, ENEMY_BAT },
    .enemy_positions = {
        { INT_TO_F88(-5), INT_TO_F88(2), INT_TO_F88(-3) },
        { INT_TO_F88( 5), INT_TO_F88(2), INT_TO_F88( 3) },
        { INT_TO_F88( 0), INT_TO_F88(5), INT_TO_F88( 0) }
    },
    .num_enemies = 3,
    .item_types = { 1, 2, 3 },  /* potions + key */
    .item_positions = {
        { INT_TO_F88(-8), INT_TO_F88(1), INT_TO_F88(0) },
        { INT_TO_F88( 8), INT_TO_F88(1), INT_TO_F88(0) },
        { INT_TO_F88( 0), INT_TO_F88(1), INT_TO_F88( 0) }
    },
    .num_items = 3,
    .door_locked = TRUE,
};

static const Room* all_rooms[NUM_ROOMS] = {
    &room0, &room1, &room2
};

void room_data_init(void) {
    /* Nothing to init - all data is const */
}

const Room* room_data_get(s32 room_id) {
    if (room_id >= 0 && room_id < NUM_ROOMS) {
        return all_rooms[room_id];
    }
    return all_rooms[0];
}

/*
 * Render room geometry: transform and submit triangles
 */
void room_data_render(s32 room_id) {
    const Room* room = room_data_get(room_id);
    if (room == NULL) return;

    const Vec3* verts;
    const Face* faces;
    s32 num_faces;

    switch (room_id) {
        case 0:
            verts = room0_verts;
            faces = room0_faces;
            num_faces = ARRAY_SIZE(room0_faces);
            break;
        case 1:
            verts = room1_verts;
            faces = room1_faces;
            num_faces = ARRAY_SIZE(room1_faces);
            break;
        case 2:
            verts = room2_verts;
            faces = room2_faces;
            num_faces = ARRAY_SIZE(room2_faces);
            break;
        default:
            return;
    }

    /* Render pillars in room 2 */
    if (room_id == 2) {
        const Model* pillar = model_get(MODEL_PILLAR);
        if (pillar) {
            Vec3 pillar_positions[] = {
                { INT_TO_F88(-5), INT_TO_F88(0), INT_TO_F88(-5) },
                { INT_TO_F88( 5), INT_TO_F88(0), INT_TO_F88(-5) },
                { INT_TO_F88(-5), INT_TO_F88(0), INT_TO_F88( 5) },
                { INT_TO_F88( 5), INT_TO_F88(0), INT_TO_F88( 5) },
            };
            s32 i;
            for (i = 0; i < 4; i++) {
                model_render(pillar, pillar_positions[i], 0, 0);
            }
        }
    }

    /* Submit room faces */
    /* Room geometry is in world space, no model transform needed */
    s32 i;
    for (i = 0; i < num_faces; i++) {
        const Face* f = &faces[i];
        Vec3 v0 = verts[f->v[0]];
        Vec3 v1 = verts[f->v[1]];
        Vec3 v2 = verts[f->v[2]];
        render_triangle(v0, v1, v2, f->color);
    }
}
