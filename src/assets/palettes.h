/*
 * Phantom Blade - Palette Data
 * Not needed for Mode 5 (direct 16-bit color)
 * Kept for potential sprite palette usage
 */

#ifndef PALETTES_H
#define PALETTES_H

#include "gba_types.h"

/* Main game palette (used if any 8-bit modes needed) */
extern const u16 main_palette[256];

void palettes_init(void);

#endif /* PALETTES_H */
