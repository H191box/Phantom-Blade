/*
 * Phantom Blade - GBA Register Definitions
 * GBA hardware register addresses for Mode 5 bitmap rendering
 */

#ifndef GBA_HEADER_H
#define GBA_HEADER_H

#include "gba_types.h"

/* ========================================================================
 * Memory Map
 * ======================================================================== */
#define REG_BASE       0x04000000
#define BIOS_BASE      0x00000000
#define EWRAM_BASE     0x02000000
#define IWRAM_BASE     0x03000000
#define PALRAM_BASE    0x05000000
#define VRAM_BASE      0x06000000
#define OAM_BASE       0x07000000
#define ROM_BASE       0x08000000

/* Mode 5 Framebuffers */
#define VRAM_FRONT     ((volatile u16*)0x06000000)
#define VRAM_BACK      ((volatile u16*)0x0600A000)
#define FB_SIZE_BYTES  40960  /* 160 * 128 * 2 bytes */

/* ========================================================================
 * Display Registers
 * ======================================================================== */
#define REG_DISPCNT    (*(volatile u32*)(REG_BASE + 0x000))  /* Display control */
#define REG_DISPSTAT   (*(volatile u32*)(REG_BASE + 0x004))  /* Display status */
#define REG_VCOUNT     (*(volatile u16*)(REG_BASE + 0x006))  /* Vertical counter */

/* ========================================================================
 * Display Control Bits
 * ======================================================================== */
#define DISPCNT_MODE0      0x0000
#define DISPCNT_MODE1      0x0001
#define DISPCNT_MODE2      0x0002
#define DISPCNT_MODE3      0x0003
#define DISPCNT_MODE4      0x0004
#define DISPCNT_MODE5      0x0005
#define DISPCNT_BG0        0x0100
#define DISPCNT_BG1        0x0200
#define DISPCNT_BG2        0x0400
#define DISPCNT_BG3        0x0800
#define DISPCNT_OBJ        0x1000
#define DISPCNT_WIN0       0x2000
#define DISPCNT_WIN1       0x4000
#define DISPCNT_OBJWIN     0x8000

/* Display status flags */
#define DISPSTAT_VBLANK    0x0001
#define DISPSTAT_HBLANK    0x0002
#define DISPSTAT_VCOUNT    0x0004
#define DISPSTAT_VBL_IRQ   0x0008
#define DISPSTAT_HBL_IRQ   0x0010
#define DISPSTAT_VCT_IRQ   0x0020

/* ========================================================================
 * Background Registers (Mode 5 uses BG2)
 * ======================================================================== */
#define REG_BG2CNT     (*(volatile u16*)(REG_BASE + 0x008))
#define REG_BG2X_L     (*(volatile u16*)(REG_BASE + 0x028))
#define REG_BG2X_H     (*(volatile u16*)(REG_BASE + 0x02A))
#define REG_BG2Y_L     (*(volatile u16*)(REG_BASE + 0x02C))
#define REG_BG2Y_H     (*(volatile u16*)(REG_BASE + 0x02E))
#define REG_BG2PA      (*(volatile u16*)(REG_BASE + 0x020))
#define REG_BG2PB      (*(volatile u16*)(REG_BASE + 0x022))
#define REG_BG2PC      (*(volatile u16*)(REG_BASE + 0x024))
#define REG_BG2PD      (*(volatile u16*)(REG_BASE + 0x026))

/* ========================================================================
 * DMA Registers
 * ======================================================================== */
#define REG_DMA0SAD    (*(volatile u32*)(REG_BASE + 0x0B0))
#define REG_DMA0DAD    (*(volatile u32*)(REG_BASE + 0x0B4))
#define REG_DMA0CNT    (*(volatile u32*)(REG_BASE + 0x0B8))
#define REG_DMA1SAD    (*(volatile u32*)(REG_BASE + 0x0BC))
#define REG_DMA1DAD    (*(volatile u32*)(REG_BASE + 0x0C0))
#define REG_DMA1CNT    (*(volatile u32*)(REG_BASE + 0x0C4))
#define REG_DMA2SAD    (*(volatile u32*)(REG_BASE + 0x0C8))
#define REG_DMA2DAD    (*(volatile u32*)(REG_BASE + 0x0CC))
#define REG_DMA2CNT    (*(volatile u32*)(REG_BASE + 0x0D0))
#define REG_DMA3SAD    (*(volatile u32*)(REG_BASE + 0x0D4))
#define REG_DMA3DAD    (*(volatile u32*)(REG_BASE + 0x0D8))
#define REG_DMA3CNT    (*(volatile u32*)(REG_BASE + 0x0DC))

/* DMA Control Flags */
#define DMA_ENABLE     0x80000000
#define DMA_IRQ        0x40000000
#define DMA_16          0x00000000
#define DMA_32          0x04000000
#define DMA_REPEAT     0x02000000
#define DMA_NOW        0x00000000
#define DMA_VBLANK     0x01000000
#define DMA_HBLANK     0x00800000
#define DMA_FIFO       0x00400000
#define DMA_SRC_INC    0x00000000
#define DMA_SRC_DEC    0x00200000
#define DMA_SRC_FIXED  0x00400000
#define DMA_SRC_RESET  0x00600000
#define DMA_DST_INC    0x00000000
#define DMA_DST_DEC    0x00080000
#define DMA_DST_FIXED  0x00100000
#define DMA_DST_RESET  0x00180000

/* ========================================================================
 * Timer Registers
 * ======================================================================== */
#define REG_TM0D       (*(volatile u16*)(REG_BASE + 0x100))
#define REG_TM0CNT     (*(volatile u16*)(REG_BASE + 0x102))
#define REG_TM1D       (*(volatile u16*)(REG_BASE + 0x104))
#define REG_TM1CNT     (*(volatile u16*)(REG_BASE + 0x106))
#define REG_TM2D       (*(volatile u16*)(REG_BASE + 0x108))
#define REG_TM2CNT     (*(volatile u16*)(REG_BASE + 0x10A))
#define REG_TM3D       (*(volatile u16*)(REG_BASE + 0x10C))
#define REG_TM3CNT     (*(volatile u16*)(REG_BASE + 0x10E))

/* Timer control bits */
#define TIMER_ENABLE   0x0080
#define TIMER_IRQ       0x0040
#define TIMER_FREQ_1    0x0000  /* 59.5ns - 32768Hz */
#define TIMER_FREQ_2    0x0001  /* 1.18us - 16384Hz */
#define TIMER_FREQ_3    0x0002  /* 2.37us - 8192Hz */
#define TIMER_FREQ_4    0x0003  /* 4.74us - 4096Hz */
#define TIMER_CASCADE   0x0004
#define TIMER_PRESCALER 0x0003

/* ========================================================================
 * Input (Key) Registers
 * ======================================================================== */
#define REG_KEY        (*(volatile u32*)(REG_BASE + 0x130))

#define KEY_A          0x0001
#define KEY_B          0x0002
#define KEY_SELECT     0x0004
#define KEY_START      0x0008
#define KEY_RIGHT      0x0010
#define KEY_LEFT       0x0020
#define KEY_UP         0x0040
#define KEY_DOWN       0x0080
#define KEY_R          0x0100
#define KEY_L          0x0200
#define KEY_MASK       0x03FF

/* ========================================================================
 * Interrupt Registers
 * ======================================================================== */
#define REG_IME        (*(volatile u32*)(REG_BASE + 0x208))
#define REG_IE         (*(volatile u32*)(REG_BASE + 0x200))
#define REG_IF         (*(volatile u32*)(REG_BASE + 0x202))

/* Interrupt flags */
#define INT_VBLANK     0x0001
#define INT_HBLANK     0x0002
#define INT_VCOUNT     0x0004
#define INT_TIMER0     0x0008
#define INT_TIMER1     0x0010
#define INT_TIMER2     0x0020
#define INT_TIMER3     0x0040
#define INT_SERIAL     0x0080
#define INT_DMA0       0x0100
#define INT_DMA1       0x0200
#define INT_DMA2       0x0400
#define INT_DMA3       0x0800
#define INT_KEYPAD     0x1000
#define INT_GAMEPAK    0x2000

/* ========================================================================
 * Screen Dimensions
 * ======================================================================== */
#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  160

/* Mode 5 dimensions */
#define MODE5_WIDTH    160
#define MODE5_HEIGHT   128

/* Color macros (15-bit RGB: 0BBBBBGGGGGRRRRR) */
#define RGB(r, g, b)   ((u16)(((b) & 0x1F) << 10) | ((g) & 0x1F) << 5) | ((r) & 0x1F))
#define RGB555(r,g,b)  (((u16)(b)<<10)|((u16)(g)<<5)|((u16)(r)))
#define COLOR_BLACK    RGB555(0,0,0)
#define COLOR_WHITE    RGB555(31,31,31)
#define COLOR_RED      RGB555(31,0,0)
#define COLOR_GREEN    RGB555(0,31,0)
#define COLOR_BLUE     RGB555(0,0,31)
#define COLOR_YELLOW   RGB555(31,31,0)
#define COLOR_CYAN     RGB555(0,31,31)
#define COLOR_MAGENTA  RGB555(31,0,31)
#define COLOR_GRAY     RGB555(16,16,16)
#define COLOR_DARK_GRAY    RGB555(8,8,8)
#define COLOR_LIGHT_GRAY  RGB555(24,24,24)

#endif /* GBA_HEADER_H */
