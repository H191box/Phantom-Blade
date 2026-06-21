/*
 * Phantom Blade - Input System
 * D-pad, A/B buttons, Start/Select, L/R shoulder buttons
 */

#ifndef INPUT_H
#define INPUT_H

#include "gba_types.h"

/* Initialize input system */
void input_init(void);

/* Poll current key state (read REG_KEY) */
void input_update(void);

/* Query individual keys */
u32 input_get_keys(void);           /* All currently pressed keys */
u32 input_get_keys_pressed(void);   /* Keys pressed this frame (just pressed) */
u32 input_get_keys_released(void);  /* Keys released this frame (just released) */

/* Individual key queries */
bool input_key_a(void);
bool input_key_b(void);
bool input_key_start(void);
bool input_key_select(void);
bool input_key_up(void);
bool input_key_down(void);
bool input_key_left(void);
bool input_key_right(void);
bool input_key_l(void);
bool input_key_r(void);

/* Direction vector for D-pad (8-way) */
s32 input_dir_x(void);  /* -1, 0, or 1 */
s32 input_dir_y(void);  /* -1, 0, or 1 */

#endif /* INPUT_H */
