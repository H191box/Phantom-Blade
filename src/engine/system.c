/*
 * Phantom Blade - GBA System Initialization
 * Mode 5 (160x128, 16-bit, double buffered bitmap mode)
 */

#include "gba_header.h"
#include "gba_types.h"
#include "system.h"
#include "dma.h"

/* Framebuffer pointers */
static u16* fb_back  = (u16*)VRAM_BACK;
static u16* fb_front = (u16*)VRAM_FRONT;
static u32 frame_count = 0;

/* VBlank handler - swaps buffers */
static void vblank_handler(void) {
    frame_count++;
}

void sys_init(void) {
    /* Initialize DMA system first */
    dma_init();

    /* Set display to Mode 5: 160x128, 16-bit color, double buffered */
    REG_DISPCNT = DISPCNT_MODE5 | DISPCNT_BG2;

    /* Enable VBlank interrupt for buffer swapping */
    REG_DISPSTAT = DISPSTAT_VBL_IRQ;
    REG_IE = INT_VBLANK;
    REG_IME = 1;

    /* Clear both framebuffers to black */
    dma_fill16((u16*)VRAM_FRONT, COLOR_BLACK, MODE5_WIDTH * MODE5_HEIGHT);
    dma_fill16((u16*)VRAM_BACK, COLOR_BLACK, MODE5_WIDTH * MODE5_HEIGHT);

    frame_count = 0;
}

void sys_wait_vblank(void) {
    /* Spin until VBlank flag is set */
    while (!(REG_DISPSTAT & DISPSTAT_VBLANK));
    /* Wait until VBlank ends */
    while (REG_DISPSTAT & DISPSTAT_VBLANK);
}

void sys_swap_buffers(void) {
    /* Toggle which framebuffer is displayed via DISPCNT bit 4 */
    REG_DISPCNT ^= 0x0010;

    /* Swap our pointers */
    u16* tmp = fb_front;
    fb_front = fb_back;
    fb_back = tmp;
}

u16* sys_get_back_buffer(void) {
    return fb_back;
}

u16* sys_get_front_buffer(void) {
    return fb_front;
}

void sys_enable_vblank_irq(void) {
    REG_DISPSTAT |= DISPSTAT_VBL_IRQ;
    REG_IE |= INT_VBLANK;
    REG_IME = 1;
}

void sys_disable_vblank_irq(void) {
    REG_DISPSTAT &= ~DISPSTAT_VBL_IRQ;
    REG_IE &= ~INT_VBLANK;
}

void sys_delay_frames(u32 frames) {
    for (u32 i = 0; i < frames; i++) {
        sys_wait_vblank();
    }
}

u32 sys_get_frame_count(void) {
    return frame_count;
}
