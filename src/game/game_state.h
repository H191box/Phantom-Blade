/*
 * Phantom Blade - Game State Machine
 * Manages overall game flow: title, playing, inventory, etc.
 */

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "gba_types.h"

/* Game states */
typedef enum {
    STATE_TITLE = 0,
    STATE_PLAYING,
    STATE_INVENTORY,
    STATE_DIALOG,
    STATE_ROOM_TRANSITION,
    STATE_GAME_OVER,
    STATE_VICTORY,
    STATE_PAUSED
} GameState;

/* Initialize game state system */
void game_init(void);

/* Per-frame game update */
void game_update(void);

/* Render current game state */
void game_render(void);

/* Get current state */
GameState game_get_state(void);

/* Set new state */
void game_set_state(GameState new_state);

/* Title screen update */
void game_title_update(void);
void game_title_render(void);

/* Game over screen */
void game_gameover_update(void);
void game_gameover_render(void);

/* Victory screen */
void game_victory_update(void);
void game_victory_render(void);

/* Inventory screen */
void game_inventory_update(void);
void game_inventory_render(void);

#endif /* GAME_STATE_H */
