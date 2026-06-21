/*
 * Phantom Blade - Dungeon System Implementation
 * Manages rooms, transitions, and world geometry rendering
 */

#include "dungeon.h"
#include "room_data.h"
#include "player.h"
#include "enemy.h"
#include "items.h"
#include "render.h"
#include "model.h"
#include "lighting.h"
#include "math3d.h"
#include "gba_header.h"

static s32 current_room_id = 0;
static const Room* current_room = NULL;
static bool transitioning = FALSE;
static s32 transition_timer = 0;
static s32 next_room_id = -1;
static Vec3 next_spawn_pos;

void dungeon_init(void) {
    current_room_id = 0;
    current_room = NULL;
    transitioning = FALSE;
    transition_timer = 0;
}

void dungeon_load_room(s32 room_id) {
    current_room_id = room_id;
    current_room = room_data_get(room_id);

    if (current_room == NULL) {
        /* Fallback to room 0 */
        current_room = room_data_get(0);
        current_room_id = 0;
    }

    /* Spawn enemies for this room */
    enemy_init();
    s32 i;
    for (i = 0; i < current_room->num_enemies && i < 4; i++) {
        enemy_create(current_room->enemy_spawns[i], current_room->enemy_positions[i]);
    }

    /* Spawn items */
    items_clear();
    for (i = 0; i < current_room->num_items && i < 4; i++) {
        items_spawn(current_room->item_types[i], current_room->item_positions[i]);
    }
}

const Room* dungeon_get_current_room(void) {
    return current_room;
}

s32 dungeon_check_doors(void) {
    if (current_room == NULL) return -1;

    Player* p = player_get();
    s32 i;
    for (i = 0; i < current_room->num_doors; i++) {
        const DoorLink* door = &current_room->doors[i];

        /* Check if room is locked (enemies alive) */
        if (current_room->door_locked && enemy_get_active_count() > 0) {
            continue;
        }

        /* Check distance to door position */
        /* Door positions are at the edges of the room */
        s32 door_x = 0, door_z = 0;
        s32 hw = current_room->width >> 1;
        s32 hd = current_room->depth >> 1;

        switch (i) {
            case 0: door_x = 0; door_z = -hd; break;    /* North */
            case 1: door_x = hw; door_z = 0; break;     /* East */
            case 2: door_x = 0; door_z = hd; break;     /* South */
            case 3: door_x = -hw; door_z = 0; break;    /* West */
        }

        /* Simple distance check */
        Vec3 door_pos = vec3_make(INT_TO_F88(door_x), p->position.y,
                                  INT_TO_F88(door_z));
        Vec3 diff = vec3_sub(p->position, door_pos);
        s32 dist = vec3_length(diff);

        if (dist < INT_TO_F88(2)) {
            return i;
        }
    }
    return -1;
}

void dungeon_transition(s32 door_idx) {
    if (current_room == NULL || door_idx < 0 ||
        door_idx >= current_room->num_doors) return;

    const DoorLink* door = &current_room->doors[door_idx];
    next_room_id = door->target_room;
    next_spawn_pos = door->spawn_position;
    transitioning = TRUE;
    transition_timer = 30;  /* 0.5 second transition */
}

void dungeon_render(void) {
    if (current_room == NULL) return;

    /* Render room geometry (walls, floor, ceiling) */
    /* Get room geometry from room_data.c */
    room_data_render(current_room_id);
}

void dungeon_render_items(void) {
    items_render();
}

s32 dungeon_get_floor(void) {
    return 1;  /* Single floor for now */
}

bool dungeon_is_room_cleared(void) {
    return enemy_get_active_count() == 0;
}

bool dungeon_check_wall_collision(Vec3 pos, s32 radius) {
    if (current_room == NULL) return FALSE;

    s32 hw = current_room->width >> 1;
    s32 hd = current_room->depth >> 1;

    s32 px = pos.x >> 8;
    s32 pz = pos.z >> 8;
    s32 r = radius >> 8;

    /* Keep player within room bounds */
    if (px - r < -hw || px + r > hw || pz - r < -hd || pz + r > hd) {
        return TRUE;
    }
    return FALSE;
}
