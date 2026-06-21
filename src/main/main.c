/*
 * Phantom Blade - Main Entry Point
 *
 * A 3D Action RPG for Game Boy Advance
 * Software-rendered 3D with Mode 5 double-buffered framebuffer
 *
 * Controls:
 *   D-Pad:  Move player (camera-relative)
 *   A:      Attack (melee swing)
 *   B:      Dodge roll (i-frames)
 *   L/R:    Rotate camera
 *   START:  Use health potion
 *   SELECT: Open inventory
 */

#include "gba_header.h"
#include "gba_types.h"
#include "system.h"
#include "input.h"
#include "render.h"
#include "camera.h"
#include "game_state.h"
#include "sprite_overlay.h"
#include "math3d.h"

/* ========================================================================
 * Main Loop
 * ======================================================================== */
int main(void) {
    /* Initialize GBA hardware */
    sys_init();

    /* Initialize math lookup tables */
    math_init();

    /* Initialize sprite/overlay system */
    sprite_init();

    /* Initialize font */
    extern void font_init(void);
    font_init();

    /* Initialize palettes */
    extern void palettes_init(void);
    palettes_init();

    /* Initialize texture data */
    extern void texture_init(void);
    texture_init();

    /* Initialize sprites */
    extern void sprites_init(void);
    sprites_init();

    /* Start in title state (game_init called on START press) */
    game_init();

    /* ====================================================================
     * Main Game Loop
     * 1. Read input
     * 2. Update game state
     * 3. Clear framebuffer
     * 4. Render scene
     * 5. Flush (depth sort + rasterize)
     * 6. Render HUD
     * 7. Swap buffers
     * 8. Wait for VBlank
     * ==================================================================== */
    while (1) {
        /* Update game logic (input read inside each state) */
        game_update();

        /* Render current game state */
        game_render();

        /* Swap front/back framebuffers */
        sys_swap_buffers();

        /* Wait for vertical blank (sync to display) */
        sys_wait_vblank();
    }

    return 0;  /* Never reached */
}
