/*
 * Phantom Blade - Player Character
 * Player state, stats, movement, combat, and inventory
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "gba_types.h"
#include "math3d.h"

/* Player states */
typedef enum {
    PLAYER_IDLE = 0,
    PLAYER_WALKING,
    PLAYER_ATTACKING,
    PLAYER_DODGING,
    PLAYER_HURT,
    PLAYER_DEAD,
    PLAYER_LEVEL_UP
} PlayerState;

/* Equipment slots */
typedef enum {
    EQUIP_WEAPON = 0,
    EQUIP_ARMOR,
    EQUIP_ACCESSORY,
    EQUIP_SLOTS_COUNT
} EquipSlot;

/* Item in inventory */
typedef struct {
    u8 type;       /* Item type ID */
    u8 count;      /* Stack count */
} InventoryItem;

/* Player structure */
typedef struct {
    /* 3D Position and movement */
    Vec3 position;
    Vec3 velocity;
    s32 rotation;       /* Y rotation (0-255) */

    /* Stats */
    s32 hp;
    s32 max_hp;
    s32 attack;
    s32 defense;
    s32 speed;          /* Movement speed (8.8) */
    s32 level;
    s32 xp;
    s32 xp_to_next;     /* XP needed for next level */
    s32 gold;

    /* State */
    PlayerState state;
    s32 state_timer;    /* Countdown timer for state (in frames) */

    /* Combat */
    s32 attack_timer;      /* Countdown to attack complete */
    s32 attack_cooldown;    /* Cooldown between attacks */
    s32 dodge_timer;       /* I-frames during dodge */
    s32 hurt_timer;        /* Invincibility frames when hurt */
    s32 hit_flash;         /* Flash white when hit */

    /* Equipment */
    s32 equipment[EQUIP_SLOTS_COUNT];

    /* Inventory */
    InventoryItem inventory[20];
    s32 inventory_count;

    /* Keys collected */
    s32 keys;

    /* Animation frame */
    s32 anim_frame;
    s32 anim_timer;

    /* Floor/room progress */
    s32 current_floor;
    s32 current_room;

    /* Facing direction (for attack cone) */
    s32 facing;  /* 0-255, same as rotation */
} Player;

/* Initialize player */
void player_init(void);

/* Per-frame update */
void player_update(void);

/* Render player model */
void player_render(void);

/* Player takes damage */
void player_take_damage(s32 damage);

/* Player gains XP */
void player_gain_xp(s32 amount);

/* Player level up */
void player_level_up(void);

/* Player attacks (A button) */
void player_attack(void);

/* Player dodges (B button) */
void player_dodge(void);

/* Add item to inventory */
bool player_add_item(u8 type, u8 count);

/* Remove item from inventory */
bool player_remove_item(u8 type, u8 count);

/* Check if player has item */
bool player_has_item(u8 type);

/* Use consumable item */
bool player_use_item(u8 type);

/* Get player pointer */
Player* player_get(void);

/* Get player position */
Vec3 player_get_position(void);

/* Check if player is alive */
bool player_is_alive(void);

/* Heal player */
void player_heal(s32 amount);

#endif /* PLAYER_H */
