#ifndef GAME_H
#define GAME_H

#include <cstddef>
#include <cstdint>
#include "common.h" // Needed for ALIEN_DEAD and GAME_MAX_BULLETS

// -- Game State --
enum GameState : uint8_t
{
    STATE_PLAYING = 0,
    STATE_WON = 1,
    STATE_LOST = 2
};

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
    bool from_player;
};

// --- Main Game State ---

struct Game
{
    size_t width, height;
    size_t num_aliens;
    size_t num_bullets;
    GameState state;
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
// void update_alients(Game *game, size_t bullet_width, size_t bullet_height,
// size_t alien_width, size_t alien_height);
void update_aliens(Game *game, uint8_t *death_counters);
void check_collisions(Game *game, size_t bullet_width, size_t bullet_height,
                      size_t alien_width, size_t alien_height,
                      size_t death_width, size_t *score,
                      size_t player_width, size_t player_height);

void update_alien_firing(Game *game);
void check_player_hit(Game *game, size_t player_width, size_t player_height);
void check_win_condition(Game *game);

#endif