/*
 * Phantom Blade - Real-Time Combat System
 * Melee combat: swing attacks, hit detection, knockback
 */

#ifndef COMBAT_H
#define COMBAT_H

#include "gba_types.h"
#include "math3d.h"

/* Initialize combat system */
void combat_init(void);

/* Player attack sweep */
void combat_player_attack(void);

/* Check if any enemies are in the player's attack cone */
void combat_check_hits(void);

/* Deal damage to enemy with knockback */
void combat_deal_damage_to_enemy(s32 enemy_idx, s32 damage);

/* Get combat debug info */
s32 combat_get_last_hit_enemy(void);
s32 combat_get_last_damage_dealt(void);

#endif /* COMBAT_H */
