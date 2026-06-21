/*
 * Phantom Blade - Item System Implementation
 */

#include "items.h"
#include "player.h"
#include "render.h"
#include "math3d.h"
#include "gba_header.h"

static WorldItem world_items[MAX_WORLD_ITEMS];

void items_init(void) {
    items_clear();
}

void items_clear(void) {
    s32 i;
    for (i = 0; i < MAX_WORLD_ITEMS; i++) {
        world_items[i].type = ITEM_NONE;
        world_items[i].collected = TRUE;
        world_items[i].bob_timer = 0;
    }
}

void items_spawn(ItemType type, Vec3 pos) {
    s32 i;
    for (i = 0; i < MAX_WORLD_ITEMS; i++) {
        if (world_items[i].collected || world_items[i].type == ITEM_NONE) {
            world_items[i].type = type;
            world_items[i].position = pos;
            world_items[i].collected = FALSE;
            world_items[i].bob_timer = 0;
            return;
        }
    }
}

void items_update_all(void) {
    Player* p = player_get();
    s32 i;

    for (i = 0; i < MAX_WORLD_ITEMS; i++) {
        WorldItem* item = &world_items[i];
        if (item->collected || item->type == ITEM_NONE) continue;

        /* Bob animation */
        item->bob_timer++;

        /* Check pickup distance */
        Vec3 diff = vec3_sub(p->position, item->position);
        s32 dist = vec3_length(diff);

        if (dist < INT_TO_F88(1)) {
            /* Pickup! */
            switch (item->type) {
                case ITEM_HEALTH_POTION:
                    player_add_item(1, 1);
                    break;
                case ITEM_LARGE_POTION:
                    player_add_item(2, 1);
                    break;
                case ITEM_KEY:
                    p->keys++;
                    break;
                case ITEM_XP_ORB:
                    player_gain_xp(10);
                    break;
                case ITEM_GOLD_PILE:
                    p->gold += 10;
                    break;
                case ITEM_TREASURE:
                    p->gold += 50;
                    player_gain_xp(25);
                    break;
                default:
                    break;
            }
            item->collected = TRUE;
        }
    }
}

void items_render(void) {
    s32 i;
    for (i = 0; i < MAX_WORLD_ITEMS; i++) {
        WorldItem* item = &world_items[i];
        if (item->collected || item->type == ITEM_NONE) continue;

        /* Render as a billboard: a vertical quad facing camera */
        /* Use bob animation for Y offset */
        s32 bob = (item->bob_timer % 30) < 15 ?
                  (item->bob_timer % 15) * 4 : (15 - (item->bob_timer % 15)) * 4;

        s32 y_offset = INT_TO_F88(1) + (bob << 8);

        u16 color = item_get_color(item->type);

        /* Create billboard quad */
        Vec3 center = vec3_make(item->position.x,
                                 item->position.y + y_offset,
                                 item->position.z);

        /* Simple 8-pointed diamond shape */
        s32 half = INT_TO_F88(1);
        Vec3 v0 = vec3_make(center.x - half, center.y - half, center.z);
        Vec3 v1 = vec3_make(center.x + half, center.y - half, center.z);
        Vec3 v2 = vec3_make(center.x + half, center.y + half, center.z);
        Vec3 v3 = vec3_make(center.x - half, center.y + half, center.z);

        /* Front face */
        render_triangle(v0, v1, v2, color);
        render_triangle(v0, v2, v3, color);
    }
}

const char* item_get_name(ItemType type) {
    switch (type) {
        case ITEM_HEALTH_POTION:  return "Potion";
        case ITEM_LARGE_POTION:  return "L.Potion";
        case ITEM_KEY:           return "Key";
        case ITEM_XP_ORB:        return "XP Orb";
        case ITEM_TREASURE:      return "Treasure";
        case ITEM_GOLD_PILE:     return "Gold";
        default:                 return "?";
    }
}

u16 item_get_color(ItemType type) {
    switch (type) {
        case ITEM_HEALTH_POTION:  return RGB555(31, 4, 8);     /* Red */
        case ITEM_LARGE_POTION:  return RGB555(8, 4, 31);      /* Blue */
        case ITEM_KEY:           return RGB555(31, 31, 4);      /* Yellow */
        case ITEM_XP_ORB:        return RGB555(16, 31, 8);     /* Green */
        case ITEM_TREASURE:      return RGB555(31, 20, 4);     /* Gold */
        case ITEM_GOLD_PILE:     return RGB555(24, 20, 4);     /* Dark gold */
        default:                 return RGB555(16, 16, 16);    /* Gray */
    }
}
