/*
 * Phantom Blade - Dungeon System
 * Room-based dungeon exploration with room transitions
 */

#ifndef DUNGEON_H
#define DUNGEON_H

#include "gba_types.h"
#include "math3d.h"

/* Room connection */
typedef struct {
    s32 target_room;        /* Room ID to transition to */
    Vec3 spawn_position;    /* Player position in target room */
    s32 spawn_facing;       /* Player facing in target room */
} DoorLink;

/* Room definition */
typedef struct {
    s32 id;                 /* Room ID */
    const char* name;       /* Room name for display */
    s32 width;              /* Room width (world units) */
    s32 depth;              /* Room depth (world units) */
    s32 height;             /* Room height */
    /* Walls, floor, ceiling geometry defined in room_data.c */
    s32 num_wall_faces;
    s32 num_floor_faces;
    /* Doors */
    DoorLink doors[4];      /* Max 4 doors per room */
    s32 num_doors;
    /* Enemies */
    s32 enemy_spawns[4];    /* Enemy type IDs */
    Vec3 enemy_positions[4];
    s32 num_enemies;
    /* Items */
    s32 item_types[4];
    Vec3 item_positions[4];
    s32 num_items;
    /* Next room triggers (kill all enemies → door opens) */
    bool door_locked;
} Room;

/* Room/Floor structure */
typedef struct {
    s32 floor_num;
    s32 num_rooms;
    const Room* rooms;
} DungeonFloor;

/* Initialize dungeon system */
void dungeon_init(void);

/* Load a specific room */
void dungeon_load_room(s32 room_id);

/* Get current room */
const Room* dungeon_get_current_room(void);

/* Check if player is at a door */
s32 dungeon_check_doors(void);

/* Transition to a new room */
void dungeon_transition(s32 door_idx);

/* Render current room geometry */
void dungeon_render(void);

/* Render room items (billboards) */
void dungeon_render_items(void);

/* Get dungeon floor number */
s32 dungeon_get_floor(void);

/* Is the current room cleared? (all enemies dead) */
bool dungeon_is_room_cleared(void);

/* Check collision with room walls */
bool dungeon_check_wall_collision(Vec3 pos, s32 radius);

#endif /* DUNGEON_H */
