/*
 * Phantom Blade - Dialog System
 * Simple text overlay for NPC dialog and messages
 */

#ifndef DIALOG_H
#define DIALOG_H

#include "gba_types.h"

/* Dialog state */
typedef enum {
    DIALOG_NONE = 0,
    DIALOG_SHOWING,
    DIALOG_WAIT_PRESS,
    DIALOG_DONE
} DialogState;

/* Initialize dialog system */
void dialog_init(void);

/* Show a dialog message */
void dialog_show(const char* text);

/* Per-frame update (handles text advancement) */
void dialog_update(void);

/* Render dialog box overlay */
void dialog_render(void);

/* Is dialog currently active? */
bool dialog_is_active(void);

/* Get current dialog state */
DialogState dialog_get_state(void);

#endif /* DIALOG_H */
