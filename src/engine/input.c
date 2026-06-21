/*
 * Phantom Blade - Input System
 * Reads GBA keypad register and tracks key states
 */

#include "gba_header.h"
#include "gba_types.h"
#include "input.h"

/* Key states */
static u32 keys_current = 0;
static u32 keys_previous = 0;

void input_init(void) {
    keys_current = 0;
    keys_previous = 0;
}

void input_update(void) {
    keys_previous = keys_current;
    /* REG_KEY is active-low: 0 = pressed */
    keys_current = ~REG_KEY & KEY_MASK;
}

u32 input_get_keys(void) {
    return keys_current;
}

u32 input_get_keys_pressed(void) {
    /* Keys pressed this frame that weren't pressed last frame */
    return keys_current & ~keys_previous;
}

u32 input_get_keys_released(void) {
    /* Keys released this frame that were pressed last frame */
    return keys_previous & ~keys_current;
}

bool input_key_a(void)      { return (keys_current & KEY_A) != 0; }
bool input_key_b(void)      { return (keys_current & KEY_B) != 0; }
bool input_key_start(void)   { return (keys_current & KEY_START) != 0; }
bool input_key_select(void)  { return (keys_current & KEY_SELECT) != 0; }
bool input_key_up(void)      { return (keys_current & KEY_UP) != 0; }
bool input_key_down(void)    { return (keys_current & KEY_DOWN) != 0; }
bool input_key_left(void)    { return (keys_current & KEY_LEFT) != 0; }
bool input_key_right(void)   { return (keys_current & KEY_RIGHT) != 0; }
bool input_key_l(void)       { return (keys_current & KEY_L) != 0; }
bool input_key_r(void)       { return (keys_current & KEY_R) != 0; }

s32 input_dir_x(void) {
    s32 x = 0;
    if (keys_current & KEY_RIGHT) x += 1;
    if (keys_current & KEY_LEFT)  x -= 1;
    return x;
}

s32 input_dir_y(void) {
    s32 y = 0;
    if (keys_current & KEY_DOWN) y += 1;
    if (keys_current & KEY_UP)   y -= 1;
    return y;
}
