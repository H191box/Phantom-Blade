/*
 * Phantom Blade - GBA Type Definitions
 * Basic types, fixed-point math types, and common macros
 */

#ifndef GBA_TYPES_H
#define GBA_TYPES_H

#include <stddef.h>
#include <stdint.h>

/* ========================================================================
 * Basic Types
 * ======================================================================== */
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef u32       uword;
typedef s32       sword;

typedef volatile u8   vu8;
typedef volatile u16  vu16;
typedef volatile u32  vu32;

#ifndef bool
#define bool int
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

/* ========================================================================
 * Fixed-Point Types
 * ======================================================================== */

/*
 * 8.8 fixed-point: 8 bits integer, 8 bits fraction
 * Range: -32768 to 32767.996
 * Precision: 1/256 ≈ 0.0039
 * Used for: vertex coordinates, normals, UV coords
 */
typedef s32 fixed88;

/* 8.8 conversions */
#define INT_TO_F88(v)   ((fixed88)((s32)(v) << 8))
#define F88_TO_INT(v)   ((s32)((v) >> 8))
#define F88_TO_FLOAT(v) ((float)(v) / 256.0f)
#define FLOAT_TO_F88(v) ((fixed88)((float)(v) * 256.0f))
#define F88_ONE         INT_TO_F88(1)
#define F88_HALF        INT_TO_F88(0)
#define F88_MAX         0x7FFFFFFF
#define F88_MIN         (-F88_MAX - 1)

/* 8.8 arithmetic */
#define F88_ADD(a, b)   ((a) + (b))
#define F88_SUB(a, b)   ((a) - (b))
#define F88_MUL(a, b)   (((s32)(((s64)(a) * (b)) >> 8)))
#define F88_DIV(a, b)   (((s32)(((s64)(a) << 8) / (b))))
#define F88_NEG(a)      (-(a))

/*
 * 12.20 fixed-point: 12 bits integer, 20 bits fraction
 * Range: -2048 to 2047.999999
 * Precision: ~0.000001
 * Used for: matrix transforms, perspective calculations
 */
typedef s32 fixed1220;

#define INT_TO_F1220(v)   ((fixed1220)((s32)(v) << 20))
#define F1220_TO_INT(v)   ((s32)((v) >> 20))
#define F1220_TO_F88(v)   ((fixed88)(((v) + (1<<11)) >> 12))
#define F1220_ONE         INT_TO_F1220(1)

/* 12.20 arithmetic */
#define F1220_ADD(a, b)   ((a) + (b))
#define F1220_SUB(a, b)   ((a) - (b))
#define F1220_MUL(a, b)   (((s32)(((s64)(a) * (b)) >> 20)))
#define F1220_DIV(a, b)   (((s32)(((s64)(a) << 20) / (b))))

/*
 * 16.16 fixed-point: 16 bits integer, 16 bits fraction
 * Range: -32768 to 32767.99998
 * Used for: general purpose, timers, angles
 */
typedef s32 fixed1616;

#define INT_TO_F1616(v)   ((fixed1616)((s32)(v) << 16))
#define F1616_TO_INT(v)   ((s32)((v) >> 16))
#define F1616_ONE         INT_TO_F1616(1)
#define F1616_HALF        INT_TO_F1616(0)

/* 16.16 arithmetic */
#define F1616_ADD(a, b)   ((a) + (b))
#define F1616_SUB(a, b)   ((a) - (b))
#define F1616_MUL(a, b)   (((s32)(((s64)(a) * (b)) >> 16)))
#define F1616_DIV(a, b)   (((s32)(((s64)(a) << 16) / (b))))

/*
 * 4.12 fixed-point: for depth/z-buffer values
 * Used for: depth comparisons, 1/z interpolation
 */
typedef s32 fixed412;

#define INT_TO_F412(v)   ((fixed412)((s32)(v) << 12))
#define F412_TO_INT(v)   ((s32)((v) >> 12))
#define F412_ONE         INT_TO_F412(1)

/* ========================================================================
 * Utility Macros
 * ======================================================================== */
#define MIN(a, b)      (((a) < (b)) ? (a) : (b))
#define MAX(a, b)      (((a) > (b)) ? (a) : (b))
#define CLAMP(v, lo, hi) MIN(MAX(v, lo), hi)
#define ABS(x)         (((x) < 0) ? -(x) : (x))
#define SWAP(a, b)     do { typeof(a) _tmp = (a); (a) = (b); (b) = _tmp; } while(0)

/* Array size */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Bit manipulation */
#define BIT(n)         (1 << (n))
#define SET_BIT(reg, n) ((reg) |= BIT(n))
#define CLR_BIT(reg, n) ((reg) &= ~BIT(n))

/* ========================================================================
 * Constants
 * ======================================================================== */
#define PI_F88          INT_TO_F88(3)
/* Approximate PI * 256 = 768 (3 * 256) — close enough for fixed-point */
#define PI              3.14159265f
#define DEG2RAD(d)      (((d) * 31416) / 1800000)  /* approximate deg to rad * 2^20 */
#define RAD2DEG(r)      (((r) * 1800000) / 31416)

/* Fixed-point sine table resolution (256 entries = 1 full circle) */
#define SIN_TABLE_SIZE  256
#define SIN_TABLE_SHIFT  8  /* angle units are 0-255 */

#endif /* GBA_TYPES_H */
