/*
 * Phantom Blade - Game State Machine Implementation
 * Controls overall game flow between different screens/modes
 */

#include "game_state.h"
#include "input.h"
#include "render.h"
#include "system.h"
#include "sprite_overlay.h"
#include "player.h"
#include "enemy.h"
#include "camera.h"
#include "dungeon.h"
#include "items.h"
#include "combat.h"
#include "dialog.h"
#include "hud.h"
#include "minimap.h"
#include "lighting.h"
#include "math3d.h"
#include "gba_header.h"

static GameState current_state = STATE_TITLE;
static s32 state_timer = 0;

void game_init(void) {
    current_state = STATE_TITLE;
    state_timer = 0;
}

GameState game_get_state(void) {
    return current_state;
}

void game_set_state(GameState new_state) {
    current_state = new_state;
    state_timer = 0;
}

/*
 * Per-frame game update
 */
void game_update(void) {
    state_timer++;

    switch (current_state) {
        case STATE_TITLE:
            game_title_update();
            break;

        case STATE_PLAYING:
        {
            /* Core game loop */
            input_update();

            /* Update player */
            player_update();

            /* Update camera */
            Vec3 player_pos = player_get_position();
            camera_update(player_pos);

            /* Update enemies */
            enemy_update_all();

            /* Update items */
            items_update_all();

            /* Check room transitions */
            s32 door = dungeon_check_doors();
            if (door >= 0 && (input_get_keys_pressed() & KEY_A)) {
                /* TODO: room transition */
                dialog_show("Door locked! Clear enemies first.");
            }

            /* Check dialog */
            dialog_update();

            /* Check game over */
            if (!player_is_alive()) {
                game_set_state(STATE_GAME_OVER);
            }
            break;
        }

        case STATE_INVENTORY:
            game_inventory_update();
            break;

        case STATE_DIALOG:
            dialog_update();
            input_update();
            if (!dialog_is_active()) {
                game_set_state(STATE_PLAYING);
            }
            break;

        case STATE_GAME_OVER:
            game_gameover_update();
            break;

        case STATE_VICTORY:
            game_victory_update();
            break;

        default:
            break;
    }
}

/*
 * Per-frame render
 */
void game_render(void) {
    switch (current_state) {
        case STATE_TITLE:
            game_title_render();
            break;

        case STATE_PLAYING:
        {
            /* Clear framebuffer */
            render_clear(COLOR_BLACK);

            /* Render dungeon geometry */
            dungeon_render();

            /* Render items */
            dungeon_render_items();

            /* Render enemies */
            enemy_render_all();

            /* Render player */
            player_render();

            /* Flush 3D triangles */
            render_flush();

            /* Render HUD on top */
            hud_render();

            /* Render minimap */
            minimap_render();

            /* Render dialog if active */
            if (dialog_is_active()) {
                dialog_render();
            }

            /* Level up message */
            Player* p = player_get();
            if (p->state == PLAYER_LEVEL_UP && p->state_timer > 0) {
                sprite_draw_text_shadow(60, 50, "LEVEL UP!",
                                         RGB555(31, 31, 8), COLOR_BLACK);
            }
            break;
        }

        case STATE_INVENTORY:
            game_inventory_render();
            break;

        case STATE_GAME_OVER:
            game_gameover_render();
            break;

        case STATE_VICTORY:
            game_victory_render();
            break;

        default:
            break;
    }
}

/* ========================================================================
 * Title Screen
 * ======================================================================== */

void game_title_update(void) {
    input_update();
    if (input_get_keys_pressed() & KEY_START) {
        /* Start game */
        math_init();
        lighting_init();
        model_init();
        camera_init();
        player_init();
        enemy_init();
        items_init();
        combat_init();
        dialog_init();
        dungeon_init();

        /* Load first room */
        dungeon_load_room(0);

        /* Show intro dialog */
        dialog_show("Welcome to Phantom Blade!");

        game_set_state(STATE_PLAYING);
    }
}

void game_title_render(void) {
    render_clear(RGB555(4, 2, 8));

    /* Title text */
    sprite_draw_text_shadow(32, 20, "PHANTOM", RGB555(24, 20, 28), RGB555(0, 0, 0));
    sprite_draw_text_shadow(36, 30, "BLADE", RGB555(31, 24, 8), RGB555(0, 0, 0));

    /* Subtitle */
    sprite_draw_text_shadow(24, 50, "A 3D Action RPG", RGB555(16, 16, 20), RGB555(0, 0, 0));

    /* Instructions */
    sprite_draw_text_shadow(16, 80, "D-Pad: Move", RGB555(20, 20, 24), RGB555(0, 0, 0));
    sprite_draw_text_shadow(16, 90, "A: Attack  B: Dodge", RGB555(20, 20, 24), RGB555(0, 0, 0));
    sprite_draw_text_shadow(16, 100, "L/R: Camera", RGB555(20, 20, 24), RGB555(0, 0, 0));
    sprite_draw_text_shadow(16, 110, "START: Use Potion", RGB555(20, 20, 24), RGB555(0, 0, 0));

    /* Blinking "Press Start" */
    if ((state_timer / 30) % 2 == 0) {
        sprite_draw_text_shadow(40, 55, "PRESS START", RGB555(31, 28, 8), RGB555(0, 0, 0));
    }

    render_flush();
}

/* ========================================================================
 * Game Over Screen
 * ======================================================================== */

void game_gameover_update(void) {
    input_update();
    if (state_timer > 60 && input_get_keys_pressed() & KEY_START) {
        game_set_state(STATE_TITLE);
    }
}

void game_gameover_render(void) {
    render_clear(RGB555(8, 0, 0));

    sprite_draw_text_shadow(44, 40, "GAME OVER", RGB555(31, 8, 8), RGB555(0, 0, 0));

    if (state_timer > 60) {
        sprite_draw_text_shadow(36, 60, "PRESS START", RGB555(20, 16, 16), RGB555(0, 0, 0));
    }

    render_flush();
}

/* ========================================================================
 * Victory Screen
 * ======================================================================== */

void game_victory_update(void) {
    input_update();
    if (state_timer > 60 && input_get_keys_pressed() & KEY_START) {
        game_set_state(STATE_TITLE);
    }
}

void game_victory_render(void) {
    render_clear(RGB555(4, 4, 12));

    sprite_draw_text_shadow(40, 30, "VICTORY!", RGB555(31, 31, 8), RGB555(0, 0, 0));
    sprite_draw_text_shadow(28, 50, "Dungeon Cleared!", RGB555(20, 20, 24), RGB555(0, 0, 0));

    Player* p = player_get();
    /* Display final stats (simplified) */
    sprite_draw_text_shadow(40, 70, "Floor: 1", RGB555(16, 20, 24), RGB555(0, 0, 0));

    if (state_timer > 60) {
        sprite_draw_text_shadow(36, 100, "PRESS START", RGB555(20, 16, 16), RGB555(0, 0, 0));
    }

    render_flush();
}

/* ========================================================================
 * Inventory Screen (simplified)
 * ======================================================================== */

void game_inventory_update(void) {
    input_update();
    if (input_get_keys_pressed() & (KEY_B | KEY_START)) {
        game_set_state(STATE_PLAYING);
    }
}

void game_inventory_render(void) {
    render_clear(RGB555(2, 2, 6));

    sprite_draw_text_shadow(48, 4, "INVENTORY", RGB555(24, 20, 28), RGB555(0, 0, 0));

    Player* p = player_get();

    /* Show stats */
    sprite_draw_text(8, 20, "HP:", RGB555(24, 8, 8));
    sprite_draw_text(40, 20, "ATK:", RGB555(24, 20, 8));
    sprite_draw_text(80, 20, "DEF:", RGB555(8, 8, 24));

    /* Level */
    sprite_draw_text(8, 30, "LV:", RGB555(8, 24, 8));

    /* Items */
    sprite_draw_text(8, 44, "ITEMS:", RGB555(20, 20, 24));

    /* Keys */
    char key_str[] = "Keys: 0";
    key_str[5] = '0' + p->keys;
    sprite_draw_text(8, 54, key_str, RGB555(24, 24, 8));

    /* Potion count */
    sprite_draw_text(8, 64, "Potions:", RGB555(24, 8, 12));

    sprite_draw_text(40, 110, "B:Exit", RGB555(16, 16, 20));

    render_flush();
}
