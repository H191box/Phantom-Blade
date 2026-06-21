/*
 * Phantom Blade - DMA Utilities
 * Fast memory transfers using GBA DMA channels
 * Essential for framebuffer clear, copy, and fill operations
 */

#ifndef DMA_H
#define DMA_H

#include "gba_types.h"

/* Initialize DMA system */
void dma_init(void);

/* Fill memory with a 16-bit value using DMA3 (fastest) */
void dma_fill16(void* dst, u16 value, u32 count);

/* Fill memory with a 32-bit value using DMA3 */
void dma_fill32(void* dst, u32 value, u32 count);

/* Copy memory 16-bit at a time using DMA3 */
void dma_copy16(const void* src, void* dst, u32 count);

/* Copy memory 32-bit at a time using DMA3 */
void dma_copy32(const void* src, void* dst, u32 count);

/* Copy during VBlank (use DMA0/1 with VBlank timing) */
void dma_copy16_vblank(const void* src, void* dst, u32 count);
void dma_fill16_vblank(void* dst, u16 value, u32 count);

#endif /* DMA_H */
