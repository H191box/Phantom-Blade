/*
 * Phantom Blade - Real-Time Combat Implementation
 * Melee attack cone detection and damage resolution
 */

#include "combat.h"
#include "player.h"
#include "enemy.h"
#include "math3d.h"
#include "gba_types.h"

static s32 last_hit_enemy = -1;
static s32 last_damage_dealt = 0;

/* Attack cone parameters */
#define ATTACK_RANGE    INT_TO_F88(3)    /* Max reach distance */
#define ATTACK_ANGLE    64              /* Half-angle of cone (90 degrees = 64) */
#define ATTACK_DAMAGE_BASE  10

void combat_init(void) {
    last_hit_enemy = -1;
    last_damage_dealt = 0;
}

/*
 * Player attack: sweep a cone in front of the player
 * Checks angle and distance to each enemy
 */
void combat_player_attack(void) {
    Player* p = player_get();
    last_hit_enemy = -1;
    last_damage_dealt = 0;

    s32 i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = enemy_get(i);
        if (e == NULL || !e->active || e->hp <= 0) continue;

        /* Calculate vector from player to enemy */
        Vec3 to_enemy = vec3_sub(e->position, p->position);
        s32 dist = vec3_length(to_enemy);

        /* Distance check */
        if (dist > ATTACK_RANGE) continue;

        /* Angle check: is enemy in front of player? */
        /* Player faces in direction p->facing (0-255) */
        /* Calculate angle of to_enemy vector */
        s32 enemy_angle = 0;
        if (to_enemy.z < 0) enemy_angle = 0;
        else if (to_enemy.z > 0) enemy_angle = 128;
        if (to_enemy.x < 0) enemy_angle = 64;
        else if (to_enemy.x > 0) enemy_angle = 192;

        /* Rougher angle calculation */
        if (to_enemy.x != 0 || to_enemy.z != 0) {
            /* atan2 approximation */
            s32 ax = to_enemy.x;
            s32 az = to_enemy.z;
            if (az != 0) {
                s32 ratio = (ax << 8) / az;
                enemy_angle = (s32)(ratio * 32 / 256);
                if (az < 0) enemy_angle = 128 - enemy_angle;
            }
        }
        enemy_angle &= 0xFF;

        /* Check if within attack cone */
        s32 angle_diff = ABS((s32)(enemy_angle - p->facing));
        if (angle_diff > 128) angle_diff = 256 - angle_diff;

        if (angle_diff <= ATTACK_ANGLE) {
            /* Hit! Calculate damage */
            s32 base_damage = p->attack;
            s32 random_mod = ((i * 37 + p->anim_frame * 13) % 5) - 2; /* -2 to +2 */
            s32 total_damage = MAX(1, base_damage + random_mod);

            enemy_damage(i, total_damage);

            last_hit_enemy = i;
            last_damage_dealt = total_damage;
        }
    }
}

void combat_check_hits(void) {
    /* Called by player_attack - nothing extra needed here */
}

void combat_deal_damage_to_enemy(s32 enemy_idx, s32 damage) {
    enemy_damage(enemy_idx, damage);
}

s32 combat_get_last_hit_enemy(void) {
    return last_hit_enemy;
}

s32 combat_get_last_damage_dealt(void) {
    return last_damage_dealt;
}
