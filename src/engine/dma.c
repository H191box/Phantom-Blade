/*
 * Phantom Blade - DMA Utilities
 * Uses DMA3 (highest priority) for fast VRAM operations
 */

#include "gba_header.h"
#include "gba_types.h"
#include "dma.h"

void dma_init(void) {
    /* DMA channels should be idle on boot, but disable them to be safe */
    REG_DMA0CNT = 0;
    REG_DMA1CNT = 0;
    REG_DMA2CNT = 0;
    REG_DMA3CNT = 0;
}

void dma_fill16(void* dst, u16 value, u32 count) {
    REG_DMA3SAD = (u32)&value;    /* Source = address of value (fixed) */
    REG_DMA3DAD = (u32)dst;       /* Destination */
    REG_DMA3CNT = DMA_DST_INC | DMA_SRC_FIXED | DMA_16 | DMA_ENABLE | count;
}

void dma_fill32(void* dst, u32 value, u32 count) {
    REG_DMA3SAD = (u32)&value;
    REG_DMA3DAD = (u32)dst;
    REG_DMA3CNT = DMA_DST_INC | DMA_SRC_FIXED | DMA_32 | DMA_ENABLE | count;
}

void dma_copy16(const void* src, void* dst, u32 count) {
    REG_DMA3SAD = (u32)src;
    REG_DMA3DAD = (u32)dst;
    REG_DMA3CNT = DMA_SRC_INC | DMA_DST_INC | DMA_16 | DMA_ENABLE | count;
}

void dma_copy32(const void* src, void* dst, u32 count) {
    REG_DMA3SAD = (u32)src;
    REG_DMA3DAD = (u32)dst;
    REG_DMA3CNT = DMA_SRC_INC | DMA_DST_INC | DMA_32 | DMA_ENABLE | count;
}

void dma_copy16_vblank(const void* src, void* dst, u32 count) {
    REG_DMA0SAD = (u32)src;
    REG_DMA0DAD = (u32)dst;
    REG_DMA0CNT = DMA_SRC_INC | DMA_DST_INC | DMA_16 | DMA_VBLANK | DMA_ENABLE | count;
}

void dma_fill16_vblank(void* dst, u16 value, u32 count) {
    REG_DMA0SAD = (u32)&value;
    REG_DMA0DAD = (u32)dst;
    REG_DMA0CNT = DMA_DST_INC | DMA_SRC_FIXED | DMA_16 | DMA_VBLANK | DMA_ENABLE | count;
}
