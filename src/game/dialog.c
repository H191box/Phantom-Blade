/*
 * Phantom Blade - Dialog System Implementation
 */

#include "dialog.h"
#include "input.h"
#include "render.h"
#include "sprite_overlay.h"
#include "gba_header.h"

#define DIALOG_MAX_LEN  80
#define DIALOG_X        8
#define DIALOG_Y        90
#define DIALOG_W        144
#define DIALOG_H        30
#define DIALOG_SPEED    2  /* Characters per frame */

static DialogState state = DIALOG_NONE;
static char text_buffer[DIALOG_MAX_LEN + 1];
static s32 text_pos;        /* Current display position */
static s32 display_timer;   /* Timer for character reveal */

void dialog_init(void) {
    state = DIALOG_NONE;
    text_buffer[0] = '\0';
    text_pos = 0;
    display_timer = 0;
}

void dialog_show(const char* text) {
    s32 i;
    for (i = 0; i < DIALOG_MAX_LEN && text[i] != '\0'; i++) {
        text_buffer[i] = text[i];
    }
    text_buffer[i] = '\0';
    text_pos = 0;
    display_timer = 0;
    state = DIALOG_SHOWING;
}

void dialog_update(void) {
    if (state == DIALOG_NONE) return;

    if (state == DIALOG_SHOWING) {
        display_timer++;
        if (display_timer >= DIALOG_SPEED) {
            display_timer = 0;
            if (text_buffer[text_pos] != '\0') {
                text_pos++;
            } else {
                state = DIALOG_WAIT_PRESS;
            }
        }

        /* A button to skip to full text */
        if (input_get_keys_pressed() & KEY_A) {
            /* Skip to end */
            while (text_buffer[text_pos] != '\0') text_pos++;
            state = DIALOG_WAIT_PRESS;
        }
    } else if (state == DIALOG_WAIT_PRESS) {
        if (input_get_keys_pressed() & KEY_A) {
            state = DIALOG_DONE;
        }
    }
}

void dialog_render(void) {
    if (state == DIALOG_NONE || state == DIALOG_DONE) return;

    /* Dialog box background */
    render_draw_rect(DIALOG_X, DIALOG_Y, DIALOG_W, DIALOG_H, RGB555(0, 0, 4));
    render_draw_rect_outline(DIALOG_X, DIALOG_Y, DIALOG_W, DIALOG_H, RGB555(24, 20, 12));

    /* Text */
    char display_buf[DIALOG_MAX_LEN + 1];
    s32 i;
    for (i = 0; i < text_pos && text_buffer[i] != '\0'; i++) {
        display_buf[i] = text_buffer[i];
    }
    display_buf[i] = '\0';

    sprite_draw_text_shadow(DIALOG_X + 4, DIALOG_Y + 4, display_buf,
                             RGB555(28, 28, 28), RGB555(0, 0, 0));

    /* "Press A" indicator when waiting */
    if (state == DIALOG_WAIT_PRESS) {
        sprite_draw_text(DIALOG_X + DIALOG_W - 40, DIALOG_Y + DIALOG_H - 12,
                        "A:Next", RGB555(20, 20, 16));
    }
}

bool dialog_is_active(void) {
    return state != DIALOG_NONE && state != DIALOG_DONE;
}

DialogState dialog_get_state(void) {
    return state;
}
