/*
 * Phantom Blade - Timer Utilities
 * GBA hardware timers for frame timing and profiling
 */

#ifndef TIMER_H
#define TIMER_H

#include "gba_types.h"

void timer_init(void);

/* Start timer 0 in free-running mode */
void timer_start(u32 channel);

/* Stop a timer */
void timer_stop(u32 channel);

/* Get elapsed ticks since timer started (16-bit, wraps at 65536) */
u16 timer_ticks(u32 channel);

/* Get elapsed time in microseconds (approximate) */
u32 timer_us(u32 channel);

/* Profile helpers */
void timer_profile_start(void);
u32 timer_profile_end(void);

#endif /* TIMER_H */
