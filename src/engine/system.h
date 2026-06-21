/*
 * Phantom Blade - GBA System Initialization
 * Mode 5 (160x128 16-bit double buffered) setup
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include "gba_types.h"

/* Initialize GBA hardware for Mode 5 rendering */
void sys_init(void);

/* Wait for VBlank interrupt */
void sys_wait_vblank(void);

/* Swap front/back framebuffers */
void sys_swap_buffers(void);

/* Get pointer to current back buffer (for drawing) */
u16* sys_get_back_buffer(void);

/* Get pointer to current front buffer (displayed) */
u16* sys_get_front_buffer(void);

/* Enable/disable VBlank IRQ */
void sys_enable_vblank_irq(void);
void sys_disable_vblank_irq(void);

/* Simple delay (busy wait) */
void sys_delay_frames(u32 frames);

/* Return current frame count */
u32 sys_get_frame_count(void);

#endif /* SYSTEM_H */
