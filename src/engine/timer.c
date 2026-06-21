/*
 * Phantom Blade - Timer Utilities
 * Uses hardware timers for frame timing and performance profiling
 */

#include "gba_header.h"
#include "gba_types.h"
#include "timer.h"

#define TIMER_CYCLES_PER_US 16  /* ~16.78MHz / 1MHz = ~16 cycles per us */

void timer_init(void) {
    /* Stop all timers */
    REG_TM0CNT = 0;
    REG_TM1CNT = 0;
    REG_TM2CNT = 0;
    REG_TM3CNT = 0;
}

void timer_start(u32 channel) {
    switch (channel) {
        case 0:
            REG_TM0D = 0;
            REG_TM0CNT = TIMER_ENABLE | TIMER_FREQ_1;
            break;
        case 1:
            REG_TM1D = 0;
            REG_TM1CNT = TIMER_ENABLE | TIMER_FREQ_1;
            break;
        case 2:
            REG_TM2D = 0;
            REG_TM2CNT = TIMER_ENABLE | TIMER_FREQ_1;
            break;
        case 3:
            REG_TM3D = 0;
            REG_TM3CNT = TIMER_ENABLE | TIMER_FREQ_1;
            break;
    }
}

void timer_stop(u32 channel) {
    switch (channel) {
        case 0: REG_TM0CNT = 0; break;
        case 1: REG_TM1CNT = 0; break;
        case 2: REG_TM2CNT = 0; break;
        case 3: REG_TM3CNT = 0; break;
    }
}

u16 timer_ticks(u32 channel) {
    switch (channel) {
        case 0: return REG_TM0D;
        case 1: return REG_TM1D;
        case 2: return REG_TM2D;
        case 3: return REG_TM3D;
    }
    return 0;
}

u32 timer_us(u32 channel) {
    /* Timer 0 at freq=1 (no prescaler) = 32768Hz, each tick ~30.5us */
    u16 ticks = timer_ticks(channel);
    return (u32)ticks * 31; /* Approximate microseconds */
}

static u16 profile_start_val = 0;

void timer_profile_start(void) {
    timer_start(0);
    profile_start_val = REG_TM0D;
}

u32 timer_profile_end(void) {
    u16 end = REG_TM0D;
    u16 elapsed;
    if (end < profile_start_val) {
        elapsed = (u16)(65536 - profile_start_val + end);
    } else {
        elapsed = end - profile_start_val;
    }
    return elapsed;
}
