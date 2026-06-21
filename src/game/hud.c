/*
 * Phantom Blade - HUD Implementation
 * Displays HP bar, level, floor info on top of 3D scene
 */

#include "hud.h"
#include "player.h"
#include "render.h"
#include "sprite_overlay.h"
#include "game_state.h"
#include "enemy.h"
#include "dungeon.h"
#include "minimap.h"
#include "gba_header.h"

void hud_init(void) {
    /* Nothing to init */
}

void hud_render(void) {
    Player* p = player_get();
    if (p == NULL) return;

    /* HP Bar - top left */
    sprite_draw_bar(4, 2, 50, 8,
                    p->hp, p->max_hp,
                    RGB555(8, 0, 0),       /* BG: dark red */
                    RGB555(24, 4, 4),       /* Fill: red */
                    RGB555(20, 16, 8));     /* Border: gold */

    /* HP text */
    sprite_draw_text(6, 3, "HP", RGB555(24, 8, 8));

    /* Level */
    sprite_draw_text(60, 3, "LV", RGB555(8, 24, 8));
    /* Level number (single digit) */
    char lv_char = '0' + (p->level % 10);
    sprite_draw_char(76, 3, lv_char, RGB555(16, 28, 16));

    /* XP bar */
    sprite_draw_bar(4, 12, 50, 4,
                    p->xp, p->xp_to_next,
                    RGB555(4, 4, 0),
                    RGB555(16, 24, 8),
                    RGB555(12, 12, 8));

    /* Floor indicator */
    sprite_draw_text(4, 18, "B1", RGB555(16, 16, 20));

    /* Room name */
    const Room* room = dungeon_get_current_room();
    if (room != NULL) {
        sprite_draw_text(4, 26, room->name, RGB555(16, 16, 20));
    }

    /* Enemy count */
    s32 enemies_left = enemy_get_active_count();
    if (enemies_left > 0) {
        char enemy_str[] = "E:0";
        enemy_str[2] = '0' + enemies_left;
        sprite_draw_text(90, 2, enemy_str, RGB555(24, 8, 8));
    } else {
        sprite_draw_text(90, 2, "CLEAR", RGB555(8, 24, 8));
    }

    /* Gold counter */
    char gold_str[] = "G:0";
    gold_str[2] = '0' + (p->gold % 10);
    sprite_draw_text(90, 12, gold_str, RGB555(24, 20, 8));

    /* Keys */
    if (p->keys > 0) {
        char key_str[] = "K:0";
        key_str[2] = '0' + p->keys;
        sprite_draw_text(90, 22, key_str, RGB555(24, 24, 8));
    }

    /* Select button = inventory hint */
    sprite_draw_text(4, SCREEN_H - 10, "SEL:Items", RGB555(10, 10, 12));

    /* Handle inventory toggle */
    if ((input_get_keys_pressed() & KEY_SELECT) &&
        game_get_state() == STATE_PLAYING) {
        game_set_state(STATE_INVENTORY);
    }
}
