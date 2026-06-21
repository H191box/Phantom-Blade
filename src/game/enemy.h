/*
 * Phantom Blade - Enemy System
 * 3D enemies with simple AI state machines
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "gba_types.h"
#include "math3d.h"

/* Enemy types */
typedef enum {
    ENEMY_SKELETON = 0,
    ENEMY_SLIME,
    ENEMY_BAT,
    ENEMY_GHOST,
    ENEMY_BOSS,
    ENEMY_TYPES_COUNT
} EnemyType;

/* AI States */
typedef enum {
    AI_IDLE = 0,
    AI_PATROL,
    AI_CHASE,
    AI_ATTACK,
    AI_HURT,
    AI_DEAD
} AIState;

/* Enemy structure */
typedef struct {
    EnemyType type;
    AIState ai_state;
    Vec3 position;
    Vec3 velocity;
    s32 rotation;
    s32 hp;
    s32 max_hp;
    s32 attack;
    s32 defense;
    s32 speed;
    s32 radius;        /* Bounding sphere radius */
    s32 state_timer;   /* Timer for current AI state */
    s32 hit_flash;     /* White flash frames */
    s32 xp_reward;     /* XP given when killed */
    s32 gold_reward;   /* Gold given when killed */
    u8  drop_item;      /* Item type dropped (0 = none) */
    bool active;        /* Is this enemy slot in use? */
    bool visible;       /* Currently rendered in view */
    /* Patrol path */
    Vec3 patrol_points[4];
    s32 patrol_count;
    s32 patrol_current;
} Enemy;

#define MAX_ENEMIES  8

/* Initialize enemy system */
void enemy_init(void);

/* Create a new enemy */
s32 enemy_create(EnemyType type, Vec3 pos);

/* Remove an enemy by index */
void enemy_destroy(s32 idx);

/* Update all enemies (AI + movement) */
void enemy_update_all(void);

/* Render all enemies */
void enemy_render_all(void);

/* Get enemy by index */
Enemy* enemy_get(s32 idx);

/* Damage an enemy */
void enemy_damage(s32 idx, s32 damage);

/* Check if enemy is alive */
bool enemy_is_alive(s32 idx);

/* Get number of active enemies */
s32 enemy_get_active_count(void);

/* Check collision between point and enemy bounding sphere */
bool enemy_check_collision(Vec3 point, s32 idx);

#endif /* ENEMY_H */
