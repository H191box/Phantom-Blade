/*
 * Phantom Blade - Master Header
 * Include this single header for all game subsystems
 */

#ifndef PHANTOM_BLADE_H
#define PHANTOM_BLADE_H

/* Version */
#define PHANTOM_BLADE_VERSION_MAJOR  1
#define PHANTOM_BLADE_VERSION_MINOR  0
#define PHANTOM_BLADE_VERSION_PATCH  0

/* ========================================================================
 * Engine Subsystems
 * ======================================================================== */
#include "gba_types.h"
#include "gba_header.h"
#include "system.h"
#include "input.h"
#include "dma.h"
#include "timer.h"
#include "math3d.h"

/* ========================================================================
 * Render System
 * ======================================================================== */
#include "rasterizer.h"
#include "rasterizer_gouraud.h"
#include "render.h"
#include "model.h"
#include "camera.h"
#include "lighting.h"
#include "sprite_overlay.h"
#include "minimap.h"

/* ========================================================================
 * Game Logic
 * ======================================================================== */
#include "player.h"
#include "enemy.h"
#include "combat.h"
#include "dungeon.h"
#include "room_data.h"
#include "items.h"
#include "dialog.h"
#include "game_state.h"
#include "hud.h"

/* ========================================================================
 * Asset Data
 * ======================================================================== */
#include "models_player.h"
#include "models_enemies.h"
#include "models_world.h"
#include "palettes.h"
#include "sprites.h"
#include "font.h"
#include "texture_data.h"

#endif /* PHANTOM_BLADE_H */
