#ifndef SPRITES_H
#define SPRITES_H

#include <cstdint>

// --- Text and UI ---
// 5x7 Font map (65 characters)
extern const uint8_t TEXT_SPRITESHEET_DATA[];

// --- Alien Assets ---
// Different types (A, B, C) and their animation frames
extern const uint8_t ALIEN_SPRITE_0_DATA[]; // Type A - Frame 1
extern const uint8_t ALIEN_SPRITE_1_DATA[]; // Type A - Frame 2
extern const uint8_t ALIEN_SPRITE_2_DATA[]; // Type B - Frame 1
extern const uint8_t ALIEN_SPRITE_3_DATA[]; // Type B - Frame 2
extern const uint8_t ALIEN_SPRITE_4_DATA[]; // Type C - Frame 1
extern const uint8_t ALIEN_SPRITE_5_DATA[]; // Type C - Frame 2

extern const uint8_t ALIEN_DEATH_SPRITE_DATA[];

// --- Player Assets ---
extern const uint8_t PLAYER_SPRITE_DATA[];
extern const uint8_t BULLET_SPRITE_DATA[];

#endif