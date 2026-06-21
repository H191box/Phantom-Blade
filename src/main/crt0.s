/*
 * Phantom Blade - GBA ROM Header Stub
 * Provides _start entry point and vector table for GBA
 */

.section ".text"
.global _start
.arm

_start:
    /* GBA entry point: jump to main in Thumb mode */
    adr r0, thumb_start
    bx r0

.thumb
thumb_start:
    /* Set up stack pointer to top of IWRAM */
    ldr r0, =0x03007F00
    mov sp, r0

    /* Jump to C main() */
    bl main

    /* Infinite loop if main returns (shouldn't happen) */
hang:
    b hang
