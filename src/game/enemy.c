/*
 * Phantom Blade - Enemy System Implementation
 * Simple AI with state machine: idle → patrol → chase → attack
 */

#include "enemy.h"
#include "player.h"
#include "render.h"
#include "model.h"
#include "lighting.h"
#include "math3d.h"
#include "gba_header.h"

static Enemy enemies[MAX_ENEMIES];

void enemy_init(void) {
    s32 i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = FALSE;
        enemies[i].hp = 0;
    }
}

/*
 * Create an enemy at a given position
 * Returns index, or -1 if no slots available
 */
s32 enemy_create(EnemyType type, Vec3 pos) {
    s32 i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            Enemy* e = &enemies[i];
            e->type = type;
            e->position = pos;
            e->velocity = vec3_make(0, 0, 0);
            e->rotation = 0;
            e->hit_flash = 0;
            e->active = TRUE;
            e->visible = TRUE;

            /* Set patrol points around spawn */
            e->patrol_count = 2;
            e->patrol_current = 0;
            e->patrol_points[0] = pos;
            e->patrol_points[1] = vec3_make(
                pos.x + INT_TO_F88(4), pos.y, pos.z
            );

            switch (type) {
                case ENEMY_SKELETON:
                    e->hp = 30; e->max_hp = 30;
                    e->attack = 8; e->defense = 1;
                    e->speed = INT_TO_F88(1); e->radius = INT_TO_F88(1);
                    e->xp_reward = 15; e->gold_reward = 5;
                    e->drop_item = 1; /* health potion */
                    break;
                case ENEMY_SLIME:
                    e->hp = 20; e->max_hp = 20;
                    e->attack = 5; e->defense = 2;
                    e->speed = INT_TO_F88(0); e->radius = INT_TO_F88(1);
                    e->xp_reward = 8; e->gold_reward = 2;
                    e->drop_item = 0;
                    break;
                case ENEMY_BAT:
                    e->hp = 15; e->max_hp = 15;
                    e->attack = 6; e->defense = 0;
                    e->speed = INT_TO_F88(1); e->radius = INT_TO_F88(1);
                    e->xp_reward = 10; e->gold_reward = 3;
                    e->drop_item = 0;
                    e->position.y = INT_TO_F88(4); /* Flying */
                    break;
                case ENEMY_GHOST:
                    e->hp = 25; e->max_hp = 25;
                    e->attack = 10; e->defense = 0;
                    e->speed = INT_TO_F88(1); e->radius = INT_TO_F88(1);
                    e->xp_reward = 20; e->gold_reward = 8;
                    e->drop_item = 1;
                    break;
                case ENEMY_BOSS:
                    e->hp = 100; e->max_hp = 100;
                    e->attack = 18; e->defense = 5;
                    e->speed = INT_TO_F88(1); e->radius = INT_TO_F88(2);
                    e->xp_reward = 100; e->gold_reward = 50;
                    e->drop_item = 2; /* large potion */
                    break;
                default:
                    e->hp = 20; e->max_hp = 20;
                    e->attack = 5; e->defense = 0;
                    e->speed = INT_TO_F88(1); e->radius = INT_TO_F88(1);
                    e->xp_reward = 5; e->gold_reward = 1;
                    e->drop_item = 0;
                    break;
            }

            e->ai_state = AI_IDLE;
            e->state_timer = 30 + (i * 15); /* Stagger initial idle */

            return i;
        }
    }
    return -1;  /* No slots */
}

void enemy_destroy(s32 idx) {
    if (idx >= 0 && idx < MAX_ENEMIES) {
        enemies[idx].active = FALSE;
    }
}

Enemy* enemy_get(s32 idx) {
    if (idx >= 0 && idx < MAX_ENEMIES) return &enemies[idx];
    return NULL;
}

bool enemy_is_alive(s32 idx) {
    if (idx >= 0 && idx < MAX_ENEMIES && enemies[idx].active) {
        return enemies[idx].hp > 0;
    }
    return FALSE;
}

s32 enemy_get_active_count(void) {
    s32 count = 0;
    s32 i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active && enemies[i].hp > 0) count++;
    }
    return count;
}

void enemy_damage(s32 idx, s32 damage) {
    if (idx < 0 || idx >= MAX_ENEMIES || !enemies[idx].active) return;

    Enemy* e = &enemies[idx];
    s32 actual = MAX(1, damage - e->defense);
    e->hp -= actual;
    e->hit_flash = 8;

    /* Knockback: push enemy away from player */
    Vec3 to_enemy = vec3_sub(e->position, player_get_position());
    to_enemy = vec3_normalize(to_enemy);
    e->position.x += (s32)(((s64)to_enemy.x * INT_TO_F88(2)) >> 8);
    e->position.z += (s32)(((s64)to_enemy.z * INT_TO_F88(2)) >> 8);

    if (e->hp <= 0) {
        e->hp = 0;
        e->ai_state = AI_DEAD;
        e->state_timer = 30;
        /* Reward player */
        player_gain_xp(e->xp_reward);
        /* TODO: add gold and item drop */
    } else {
        e->ai_state = AI_HURT;
        e->state_timer = 15;
    }
}

bool enemy_check_collision(Vec3 point, s32 idx) {
    if (idx < 0 || idx >= MAX_ENEMIES || !enemies[idx].active) return FALSE;
    Enemy* e = &enemies[idx];
    Vec3 diff = vec3_sub(point, e->position);
    /* Simple distance check using 8.8 fixed-point */
    s32 dist_sq = (s32)((s64)diff.x * diff.x + (s64)diff.y * diff.y +
                         (s64)diff.z * diff.z) >> 8;
    s32 radius_sq = e->radius >> 8;
    return dist_sq < (radius_sq * radius_sq);
}

/*
 * Update all enemies (AI + movement)
 */
void enemy_update_all(void) {
    s32 i;
    Vec3 player_pos = player_get_position();

    for (i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = &enemies[i];
        if (!e->active) continue;

        if (e->hit_flash > 0) e->hit_flash--;
        if (e->state_timer > 0) e->state_timer--;

        /* Dead enemies: count down and remove */
        if (e->ai_state == AI_DEAD) {
            if (e->state_timer <= 0) {
                e->active = FALSE;
            }
            continue;
        }

        /* Calculate distance to player */
        Vec3 to_player = vec3_sub(player_pos, e->position);
        s32 dist = vec3_length(to_player);

        /* AI State Machine */
        switch (e->ai_state) {
            case AI_IDLE:
                if (e->state_timer <= 0) {
                    e->ai_state = AI_PATROL;
                    e->patrol_current = 0;
                }
                break;

            case AI_PATROL:
                /* Move toward current patrol point */
                {
                    Vec3 target = e->patrol_points[e->patrol_current];
                    Vec3 to_target = vec3_sub(target, e->position);
                    s32 t_dist = vec3_length(to_target);

                    if (t_dist < INT_TO_F88(1)) {
                        /* Reached patrol point, go to next */
                        e->patrol_current = (e->patrol_current + 1) % e->patrol_count;
                        e->ai_state = AI_IDLE;
                        e->state_timer = 30;
                    } else {
                        /* Move toward target */
                        to_target = vec3_normalize(to_target);
                        e->position.x += (s32)(((s64)to_target.x * e->speed) >> 8);
                        e->position.z += (s32)(((s64)to_target.z * e->speed) >> 8);

                        /* Update facing */
                        e->rotation = e->patrol_current * 64;
                    }

                    /* If player is close, switch to chase */
                    if (dist < INT_TO_F88(8)) {
                        e->ai_state = AI_CHASE;
                    }
                }
                break;

            case AI_CHASE:
                if (dist > INT_TO_F88(12)) {
                    /* Player too far, return to patrol */
                    e->ai_state = AI_IDLE;
                    e->state_timer = 60;
                } else if (dist < INT_TO_F88(2)) {
                    /* Close enough to attack */
                    e->ai_state = AI_ATTACK;
                    e->state_timer = 30;
                } else {
                    /* Move toward player */
                    Vec3 dir = vec3_normalize(to_player);
                    e->position.x += (s32)(((s64)dir.x * e->speed) >> 8);
                    e->position.z += (s32)(((s64)dir.z * e->speed) >> 8);

                    /* Face player */
                    if (dir.z != 0 || dir.x != 0) {
                        s32 angle = 0;
                        if (dir.z < 0) angle = 0;
                        else if (dir.z > 0) angle = 128;
                        if (dir.x < 0) angle = 64;
                        else if (dir.x > 0) angle = 192;
                        e->rotation = angle;
                    }
                }
                break;

            case AI_ATTACK:
                if (e->state_timer == 20) {
                    /* Deal damage at mid-attack frame */
                    Player* pl = player_get();
                    s32 dist_to_player = vec3_length(to_player);
                    if (dist_to_player < INT_TO_F88(2)) {
                        pl->take_damage(e->attack);
                    }
                }
                if (e->state_timer <= 0) {
                    e->ai_state = AI_CHASE;
                }
                break;

            case AI_HURT:
                if (e->state_timer <= 0) {
                    if (dist < INT_TO_F88(8)) {
                        e->ai_state = AI_CHASE;
                    } else {
                        e->ai_state = AI_PATROL;
                    }
                }
                break;
        }

        /* Keep enemies on ground (Y = 2 units) */
        if (e->type != ENEMY_BAT) {
            e->position.y = INT_TO_F88(2);
        }
    }
}

/*
 * Render all active enemies
 */
void enemy_render_all(void) {
    s32 i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = &enemies[i];
        if (!e->active || e->hp <= 0) continue;

        /* Select model based on type */
        ModelID mdl_id;
        switch (e->type) {
            case ENEMY_SKELETON: mdl_id = MODEL_SKELETON; break;
            case ENEMY_SLIME:    mdl_id = MODEL_SLIME; break;
            case ENEMY_BAT:      mdl_id = MODEL_BAT; break;
            case ENEMY_GHOST:    mdl_id = MODEL_SLIME; break; /* Use slime for now */
            case ENEMY_BOSS:     mdl_id = MODEL_SKELETON; break; /* Use skeleton for now */
            default:             mdl_id = MODEL_SLIME; break;
        }

        const Model* mdl = model_get(mdl_id);
        if (mdl == NULL) continue;

        /* Flash white if hit */
        u16 tint = (e->hit_flash > 0 && e->hit_flash % 2 == 0) ?
                   RGB555(31, 31, 31) : 0;

        model_render(mdl, e->position, e->rotation, tint);
    }
}
