/*
 * Phantom Blade - HUD (Heads-Up Display)
 * Displays player stats, HP bar, floor info, minimap
 * Rendered on top of 3D framebuffer
 */

#ifndef HUD_H
#define HUD_H

#include "gba_types.h"

/* Initialize HUD */
void hud_init(void);

/* Render HUD overlay */
void hud_render(void);

#endif /* HUD_H */
