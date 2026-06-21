/*
 * Phantom Blade - Item System
 * Pickable items in the dungeon world
 */

#ifndef ITEMS_H
#define ITEMS_H

#include "gba_types.h"
#include "math3d.h"

/* Item types */
typedef enum {
    ITEM_NONE = 0,
    ITEM_HEALTH_POTION,
    ITEM_LARGE_POTION,
    ITEM_KEY,
    ITEM_XP_ORB,
    ITEM_TREASURE,
    ITEM_GOLD_PILE
} ItemType;

/* World item (sitting on the ground) */
typedef struct {
    ItemType type;
    Vec3 position;
    bool collected;
    s32 bob_timer;  /* Animation: bob up and down */
} WorldItem;

#define MAX_WORLD_ITEMS  8

/* Initialize items */
void items_init(void);

/* Clear all world items */
void items_clear(void);

/* Spawn an item in the world */
void items_spawn(ItemType type, Vec3 pos);

/* Update all items (animation, pickup check) */
void items_update_all(void);

/* Render all items */
void items_render(void);

/* Item type names (for HUD display) */
const char* item_get_name(ItemType type);

/* Item type colors (for rendering) */
u16 item_get_color(ItemType type);

#endif /* ITEMS_H */
