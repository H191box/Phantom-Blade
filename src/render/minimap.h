/*
 * Phantom Blade - Minimap System
 * Top-down 2D minimap rendered in corner of framebuffer
 */

#ifndef MINIMAP_H
#define MINIMAP_H

#include "gba_types.h"
#include "math3d.h"

/* Minimap configuration */
#define MINIMAP_SIZE  32   /* 32x32 pixel minimap */
#define MINIMAP_X     124  /* Position on screen (right side) */
#define MINIMAP_Y     2

/* Minimap cell types */
typedef enum {
    MM_EMPTY = 0,
    MM_WALL,
    MM_FLOOR,
    MM_DOOR,
    MM_PLAYER,
    MM_ENEMY,
    MM_ITEM,
    MM_EXIT
} MinimapCell;

/* Initialize minimap */
void minimap_init(void);

/* Clear minimap data */
void minimap_clear(void);

/* Set a cell on the minimap */
void minimap_set_cell(s32 x, s32 z, MinimapCell cell);

/* Set player position on minimap */
void minimap_set_player(s32 x, s32 z);

/* Set enemy positions */
void minimap_set_enemy(s32 idx, s32 x, s32 z);
void minimap_clear_enemies(void);

/* Set item positions */
void minimap_set_item(s32 idx, s32 x, s32 z);
void minimap_clear_items(void);

/* Render minimap to framebuffer */
void minimap_render(void);

#endif /* MINIMAP_H */
