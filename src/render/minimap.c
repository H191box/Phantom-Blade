/*
 * Phantom Blade - Minimap Implementation
 * Renders a small top-down view in the top-right corner
 */

#include "minimap.h"
#include "render.h"
#include "gba_header.h"

/* Minimap grid data (represents a small area around the player) */
#define MM_GRID  32
static u8 mm_grid[MM_GRID][MM_GRID];
static s32 mm_player_x = MM_GRID / 2;
static s32 mm_player_z = MM_GRID / 2;
static s32 mm_enemy_x[8];
static s32 mm_enemy_z[8];
static s32 mm_item_x[8];
static s32 mm_item_z[8];
static s32 num_enemies_shown = 0;
static s32 num_items_shown = 0;

/* Color palette for minimap cells */
static const u16 mm_colors[] = {
    RGB555(0, 0, 0),      /* EMPTY - black */
    RGB555(16, 16, 20),   /* WALL - dark gray */
    RGB555(6, 6, 8),      /* FLOOR - very dark gray */
    RGB555(20, 16, 8),    /* DOOR - brown */
};

void minimap_init(void) {
    minimap_clear();
}

void minimap_clear(void) {
    s32 x, z;
    for (x = 0; x < MM_GRID; x++) {
        for (z = 0; z < MM_GRID; z++) {
            mm_grid[x][z] = MM_EMPTY;
        }
    }
    mm_player_x = MM_GRID / 2;
    mm_player_z = MM_GRID / 2;
    num_enemies_shown = 0;
    num_items_shown = 0;
}

void minimap_set_cell(s32 x, s32 z, MinimapCell cell) {
    if (x >= 0 && x < MM_GRID && z >= 0 && z < MM_GRID) {
        mm_grid[x][z] = cell;
    }
}

void minimap_set_player(s32 x, s32 z) {
    mm_player_x = CLAMP(x, 0, MM_GRID - 1);
    mm_player_z = CLAMP(z, 0, MM_GRID - 1);
}

void minimap_set_enemy(s32 idx, s32 x, s32 z) {
    if (idx < 8) {
        mm_enemy_x[idx] = CLAMP(x, 0, MM_GRID - 1);
        mm_enemy_z[idx] = CLAMP(z, 0, MM_GRID - 1);
        if (idx >= num_enemies_shown) num_enemies_shown = idx + 1;
    }
}

void minimap_clear_enemies(void) {
    num_enemies_shown = 0;
}

void minimap_set_item(s32 idx, s32 x, s32 z) {
    if (idx < 8) {
        mm_item_x[idx] = CLAMP(x, 0, MM_GRID - 1);
        mm_item_z[idx] = CLAMP(z, 0, MM_GRID - 1);
        if (idx >= num_items_shown) num_items_shown = idx + 1;
    }
}

void minimap_clear_items(void) {
    num_items_shown = 0;
}

/*
 * Render minimap
 * Draw it in the top-right corner of the Mode 5 framebuffer
 * Each grid cell = 1 pixel (32x32 minimap)
 */
void minimap_render(void) {
    s32 x, z;
    /* Draw background */
    render_draw_rect(MINIMAP_X - 1, MINIMAP_Y - 1, MM_GRID + 2, MM_GRID + 2,
                     RGB555(0, 0, 0));

    /* Draw grid */
    for (x = 0; x < MM_GRID; x++) {
        for (z = 0; z < MM_GRID; z++) {
            u8 cell = mm_grid[x][z];
            if (cell <= MM_DOOR) {
                u16* fb = render_get_back_buffer();
                s32 px = MINIMAP_X + x;
                s32 py = MINIMAP_Y + z;
                if (px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H) {
                    fb[py * SCREEN_W + px] = mm_colors[cell];
                }
            }
        }
    }

    /* Draw enemies as red dots */
    s32 i;
    for (i = 0; i < num_enemies_shown; i++) {
        s32 px = MINIMAP_X + mm_enemy_x[i];
        s32 py = MINIMAP_Y + mm_enemy_z[i];
        if (px >= MINIMAP_X && px < MINIMAP_X + MM_GRID &&
            py >= MINIMAP_Y && py < MINIMAP_Y + MM_GRID) {
            render_draw_rect(px, py, 2, 2, RGB555(31, 4, 4));
        }
    }

    /* Draw items as yellow dots */
    for (i = 0; i < num_items_shown; i++) {
        s32 px = MINIMAP_X + mm_item_x[i];
        s32 py = MINIMAP_Y + mm_item_z[i];
        if (px >= MINIMAP_X && px < MINIMAP_X + MM_GRID &&
            py >= MINIMAP_Y && py < MINIMAP_Y + MM_GRID) {
            render_draw_rect(px, py, 2, 2, RGB555(31, 31, 4));
        }
    }

    /* Draw player as green dot */
    s32 ppx = MINIMAP_X + mm_player_x;
    s32 ppy = MINIMAP_Y + mm_player_z;
    if (ppx >= MINIMAP_X && ppx < MINIMAP_X + MM_GRID &&
        ppy >= MINIMAP_Y && ppy < MINIMAP_Y + MM_GRID) {
        render_draw_rect(ppx, ppy, 2, 2, RGB555(0, 31, 0));
    }

    /* Border */
    render_draw_rect_outline(MINIMAP_X - 1, MINIMAP_Y - 1,
                             MM_GRID + 2, MM_GRID + 2, RGB555(20, 20, 24));
}
