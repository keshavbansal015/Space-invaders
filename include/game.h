#ifndef GAME_H
#define GAME_H

#include <cstddef>
#include <cstdint>
#include "common.h" // Needed for ALIEN_DEAD and GAME_MAX_BULLETS

// --- Entity Structures ---

struct Alien
{
    size_t x, y;
    uint8_t type; // Uses AlienType values from common.h
};

struct Player
{
    size_t x, y;
    size_t life;
};

struct Bullet
{
    size_t x, y;
    int dir;
};

// --- Main Game State ---

struct Game
{
    size_t width, height;
    size_t num_aliens;
    size_t num_bullets;
    Alien *aliens; // Dynamically allocated in main.cpp
    Player player;
    Bullet bullets[GAME_MAX_BULLETS];
};

// --- Logic Prototypes ---
// These will be implemented in src/game.cpp later

bool sprite_overlap_check(
    size_t x_a, size_t y_a, size_t width_a, size_t height_a,
    size_t x_b, size_t y_b, size_t width_b, size_t height_b);

// Entity updates
void update_player(Game *game, int move_dir, size_t player_width);
void update_bullets(Game *game, size_t bullet_height);

#endif