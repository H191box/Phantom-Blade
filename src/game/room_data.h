/*
 * Phantom Blade - Room Data Definitions
 * Pre-built room geometry: vertices, faces, enemies, items
 * 3 distinct rooms with different layouts
 */

#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#include "gba_types.h"
#include "math3d.h"
#include "dungeon.h"

/* Number of rooms */
#define NUM_ROOMS 3

/* Initialize room data */
void room_data_init(void);

/* Get room by ID */
const Room* room_data_get(s32 room_id);

/* Render room geometry */
void room_data_render(s32 room_id);

#endif /* ROOM_DATA_H */
