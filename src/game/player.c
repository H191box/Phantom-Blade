/*
 * Phantom Blade - Player Character Implementation
 */

#include "player.h"
#include "input.h"
#include "camera.h"
#include "render.h"
#include "model.h"
#include "combat.h"
#include "math3d.h"

static Player p;

void player_init(void) {
    p.position = vec3_make(INT_TO_F88(8), INT_TO_F88(2), INT_TO_F88(8));
    p.velocity = vec3_make(0, 0, 0);
    p.rotation = 64;  /* Facing -Z direction (into screen in world space) */

    p.hp = 100;
    p.max_hp = 100;
    p.attack = 10;
    p.defense = 3;
    p.speed = INT_TO_F88(2);  /* 2 units per frame */
    p.level = 1;
    p.xp = 0;
    p.xp_to_next = 50;
    p.gold = 0;

    p.state = PLAYER_IDLE;
    p.state_timer = 0;

    p.attack_timer = 0;
    p.attack_cooldown = 0;
    p.dodge_timer = 0;
    p.hurt_timer = 0;
    p.hit_flash = 0;

    /* Clear equipment */
    s32 i;
    for (i = 0; i < EQUIP_SLOTS_COUNT; i++) {
        p.equipment[i] = -1;
    }

    /* Clear inventory */
    p.inventory_count = 0;
    for (i = 0; i < 20; i++) {
        p.inventory[i].type = 0;
        p.inventory[i].count = 0;
    }

    p.keys = 0;
    p.anim_frame = 0;
    p.anim_timer = 0;
    p.current_floor = 1;
    p.current_room = 0;
    p.facing = 64;
}

Player* player_get(void) {
    return &p;
}

Vec3 player_get_position(void) {
    return p.position;
}

bool player_is_alive(void) {
    return p.hp > 0;
}

void player_take_damage(s32 damage) {
    if (p.dodge_timer > 0 || p.hurt_timer > 0) return;

    s32 actual_damage = MAX(1, damage - p.defense);
    p.hp -= actual_damage;
    if (p.hp < 0) p.hp = 0;
    p.hurt_timer = 30;  /* 0.5 seconds invincibility */
    p.hit_flash = 8;

    if (p.hp <= 0) {
        p.state = PLAYER_DEAD;
    }
}

void player_gain_xp(s32 amount) {
    p.xp += amount;
    while (p.xp >= p.xp_to_next) {
        p.xp -= p.xp_to_next;
        player_level_up();
    }
}

void player_level_up(void) {
    p.level++;
    p.max_hp += 10;
    p.hp = p.max_hp;  /* Full heal on level up */
    p.attack += 2;
    p.defense += 1;
    p.xp_to_next = p.xp_to_next * 3 / 2;  /* 50% more XP needed */
    p.state = PLAYER_LEVEL_UP;
    p.state_timer = 60;  /* Show "LEVEL UP" for 1 second */
}

void player_heal(s32 amount) {
    p.hp = MIN(p.hp + amount, p.max_hp);
}

bool player_add_item(u8 type, u8 count) {
    /* Check for existing stack */
    s32 i;
    for (i = 0; i < p.inventory_count; i++) {
        if (p.inventory[i].type == type) {
            p.inventory[i].count += count;
            return TRUE;
        }
    }
    /* Add new slot */
    if (p.inventory_count < 20) {
        p.inventory[p.inventory_count].type = type;
        p.inventory[p.inventory_count].count = count;
        p.inventory_count++;
        return TRUE;
    }
    return FALSE;  /* Inventory full */
}

bool player_remove_item(u8 type, u8 count) {
    s32 i;
    for (i = 0; i < p.inventory_count; i++) {
        if (p.inventory[i].type == type) {
            if (p.inventory[i].count >= count) {
                p.inventory[i].count -= count;
                if (p.inventory[i].count == 0) {
                    /* Remove empty slot, shift remaining */
                    s32 j;
                    for (j = i; j < p.inventory_count - 1; j++) {
                        p.inventory[j] = p.inventory[j + 1];
                    }
                    p.inventory_count--;
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool player_has_item(u8 type) {
    s32 i;
    for (i = 0; i < p.inventory_count; i++) {
        if (p.inventory[i].type == type && p.inventory[i].count > 0) {
            return TRUE;
        }
    }
    return FALSE;
}

bool player_use_item(u8 type) {
    if (!player_has_item(type)) return FALSE;

    switch (type) {
        case 1: /* Health Potion */
            player_heal(30);
            break;
        case 2: /* Large Health Potion */
            player_heal(60);
            break;
        default:
            return FALSE;
    }
    player_remove_item(type, 1);
    return TRUE;
}

/*
 * Per-frame update: movement, state machine, timers
 */
void player_update(void) {
    /* Decrement timers */
    if (p.state_timer > 0) p.state_timer--;
    if (p.attack_timer > 0) p.attack_timer--;
    if (p.attack_cooldown > 0) p.attack_cooldown--;
    if (p.dodge_timer > 0) p.dodge_timer--;
    if (p.hurt_timer > 0) p.hurt_timer--;
    if (p.hit_flash > 0) p.hit_flash--;

    /* Animation timer */
    p.anim_timer++;
    if (p.anim_timer >= 8) {
        p.anim_timer = 0;
        p.anim_frame++;
    }

    /* State transitions */
    if (p.state == PLAYER_DEAD) return;

    if (p.state == PLAYER_LEVEL_UP && p.state_timer <= 0) {
        p.state = PLAYER_IDLE;
    }

    if (p.state == PLAYER_ATTACKING && p.attack_timer <= 0) {
        p.state = PLAYER_IDLE;
    }

    if (p.state == PLAYER_DODGING && p.dodge_timer <= 0) {
        p.state = PLAYER_IDLE;
    }

    /* Only accept input in idle/walking state */
    if (p.state == PLAYER_IDLE || p.state == PLAYER_WALKING) {
        /* A button: Attack */
        if (input_key_a() && p.attack_cooldown <= 0) {
            player_attack();
        }
        /* B button: Dodge */
        else if (input_key_b() && p.dodge_timer <= 0) {
            player_dodge();
        }
        /* START: Use health potion */
        else if (input_get_keys_pressed() & KEY_START) {
            player_use_item(1);  /* Health potion */
        }

        /* D-pad movement */
        if (p.state != PLAYER_ATTACKING && p.state != PLAYER_DODGING) {
            s32 dx = input_dir_x();
            s32 dy = input_dir_y();

            if (dx != 0 || dy != 0) {
                p.state = PLAYER_WALKING;

                /* Camera-relative movement */
                Vec3 cam_fwd = camera_get_forward();
                Vec3 cam_right = camera_get_right();

                /* Movement in camera space → world space */
                s32 move_x = (s32)(((s64)cam_right.x * (dx << 8) + (s64)cam_fwd.x * (dy << 8)) >> 16);
                s32 move_z = (s32)(((s64)cam_right.z * (dx << 8) + (s64)cam_fwd.z * (dy << 8)) >> 16);

                p.velocity.x = (move_x * p.speed) >> 8;
                p.velocity.z = (move_z * p.speed) >> 8;

                /* Update facing direction */
                if (move_x != 0 || move_z != 0) {
                    /* Calculate facing angle from movement vector */
                    s32 angle_f = 0;
                    if (move_z < 0) angle_f = 0;
                    else if (move_z > 0) angle_f = 128;
                    if (move_x < 0) angle_f = 64;
                    else if (move_x > 0) angle_f = 192;

                    /* More precise angle */
                    if (move_x != 0 || move_z != 0) {
                        /* atan2 approximation using fixed-point */
                        /* angle = atan2(-x, -z) mapped to 0-255 */
                        s32 ax = -move_x;
                        s32 az = -move_z;
                        if (az == 0) {
                            angle_f = (ax > 0) ? 64 : 192;
                        } else {
                            /* Approximate: ratio-based */
                            s32 ratio = (ax << 8) / az;
                            angle_f = (s32)(ratio * 32 / 256);  /* rough */
                            if (az < 0) angle_f = 128 - angle_f;
                        }
                    }
                    p.facing = angle_f & 0xFF;
                }
            } else {
                p.state = PLAYER_IDLE;
                p.velocity.x = 0;
                p.velocity.z = 0;
            }
        }
    }

    /* Apply velocity (simple - no collision yet) */
    if (p.state != PLAYER_DODGING) {
        p.position.x += p.velocity.x;
        p.position.z += p.velocity.z;
    }

    /* Dodge movement */
    if (p.state == PLAYER_DODGING) {
        /* Dodge in facing direction */
        s32 dodge_speed = INT_TO_F88(4);
        s32 sin_f = sin_lut(p.facing);
        s32 cos_f = cos_lut(p.facing);
        p.position.x = (s32)(((s64)sin_f * dodge_speed) >> 8) + p.position.x;
        p.position.z = (s32)(((s64)cos_f * dodge_speed) >> 8) + p.position.z;
    }
}

void player_attack(void) {
    p.state = PLAYER_ATTACKING;
    p.attack_timer = 15;       /* 0.25 second attack duration */
    p.attack_cooldown = 25;   /* ~0.4 second cooldown */
    combat_player_attack();
}

void player_dodge(void) {
    p.state = PLAYER_DODGING;
    p.dodge_timer = 15;       /* 0.25 second dodge (i-frames) */
}

void player_render(void) {
    if (p.state == PLAYER_DEAD) return;

    const Model* mdl = model_get(MODEL_PLAYER);
    if (mdl == NULL) return;

    s32 rot_y = p.rotation;

    /* Hit flash white */
    u16 tint = (p.hit_flash > 0 && p.hit_flash % 2 == 0) ?
               RGB555(31, 31, 31) : 0;

    model_render(mdl, p.position, rot_y, tint);
}
