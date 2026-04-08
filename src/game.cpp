#include "common.h"
#include "game.h"
#include <cstdlib>

bool sprite_overlap_check(
    size_t x_a, size_t y_a, size_t width_a, size_t height_a,
    size_t x_b, size_t y_b, size_t width_b, size_t height_b)
{
    if (x_a < x_b + width_b && x_a + width_a > x_b &&
        y_a < y_b + height_b && y_a + height_a > y_b)
        return true;
    return false;
}

// move_dir -> +1 = right, -1 = left, move 2 pixels at a time
void update_player(Game *game, int move_dir, size_t player_width)
{
    int player_move_speed = 2 * move_dir;
    if (player_move_speed != 0)
    {
        if (game->player.x + player_width + player_move_speed >= game->width)
            game->player.x = game->width - player_width;
        else if ((int)game->player.x + player_move_speed <= 0)
            game->player.x = 0;
        else
            game->player.x += player_move_speed;
    }
}

// bullet->dir = 2 -> Up (to the monsters),
void update_bullets(Game *game, size_t bullet_height)
{
    for (size_t bi = 0; bi < game->num_bullets;)
    {
        game->bullets[bi].y += game->bullets[bi].dir;

        // Check if bullet is out of bounds
        if (game->bullets[bi].y >= game->height || game->bullets[bi].y < bullet_height)
        {
            // Slot-map swap: replace current with last and decrement count
            game->bullets[bi] = game->bullets[game->num_bullets - 1];
            --game->num_bullets;
            // Don't increment bi so we check the swapped bullet in the next pass
            continue;
        }
        ++bi;
    }
}

void update_aliens(Game *game, uint8_t *death_counters)
{
    for (size_t ai = 0; ai < game->num_aliens; ++ai)
    {
        Alien &alien = game->aliens[ai];
        // If the alien was marked dead, decrement its display timer
        if (alien.type == ALIEN_DEAD && death_counters[ai] > 0)
        {
            --death_counters[ai];
        }
    }
}

void check_collisions(Game *game, size_t bullet_width, size_t bullet_height,
                      size_t alien_width, size_t alien_height,
                      size_t death_width, size_t *score,
                      size_t player_width, size_t player_height)
{
    for (size_t bi = 0; bi < game->num_bullets;)
    {
        bool bullet_destroyed = false;
        if (!game->bullets[bi].from_player)
        {
            if (sprite_overlap_check(game->bullets[bi].x, game->bullets[bi].y, 1, 3,
                                     game->player.x, game->player.y, player_width, player_height))
            {
                if (game->player.life > 0)
                    game->player.life--;

                // Remove bullet
                game->bullets[bi] = game->bullets[game->num_bullets - 1];
                game->num_bullets--;

                if (game->player.life == 0)
                {
                    game->state = STATE_LOST;
                    break;
                }
            }
            bi++;
            continue;
        }

        for (size_t ai = 0; ai < game->num_aliens; ++ai)
        {
            Alien &alien = game->aliens[ai];
            if (alien.type == ALIEN_DEAD)
                continue;

            if (sprite_overlap_check(
                    game->bullets[bi].x, game->bullets[bi].y, bullet_width, bullet_height,
                    alien.x, alien.y, alien_width, alien_height))
            {
                // Increase score based on alien rank (Type 3 = bottom, Type 1 = top)
                *score += 10 * (4 - alien.type);

                // Mark as dead and recenter for the death sprite
                alien.type = ALIEN_DEAD;
                alien.x -= (death_width - alien_width) / 2;

                // Remove bullet via swap-to-back
                game->bullets[bi] = game->bullets[game->num_bullets - 1];
                --game->num_bullets;

                bullet_destroyed = true;
                break; // This bullet is gone, stop checking other aliens
            }
        }

        if (!bullet_destroyed)
        {
            ++bi;
        }
    }
}

void update_alien_firing(Game *game)
{
    // Only fire sometimes 10%
    if (game->num_bullets < GAME_MAX_BULLETS && (rand() % 100 == 0))
    {
        size_t index = rand() % game->num_aliens;
        Alien &a = game->aliens[index];

        if (a.type != ALIEN_DEAD)
        {
            Bullet &b = game->bullets[game->num_bullets++];
            b.x = a.x + 5; // from alien's center
            b.y = a.y+10;
            b.dir = -2; // Moving down
            b.from_player = false;
        }
    }
}

void check_win_condition(Game *game)
{
    size_t active_aliens = 0;
    for (size_t i = 0; i < game->num_aliens; i++)
    {
        if (game->aliens[i].type != ALIEN_DEAD)
            active_aliens++;
    }
    if (active_aliens == 0)
        game->state = STATE_WON;
}