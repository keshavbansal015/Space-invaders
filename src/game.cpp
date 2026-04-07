#include "common.h"
#include "game.h"

// bool sprite_overlap_check(
//     const Sprite &sp_a, size_t x_a, size_t y_a,
//     const Sprite &sp_b, size_t x_b, size_t y_b)
// {
//     if (x_a < x_b + sp_b.width && x_a + sp_a.width > x_b &&
//         y_a < y_b + sp_b.height && y_a + sp_a.height > y_b)
//         return true;
//     return false;
// }
bool sprite_overlap_check(
    size_t x_a, size_t y_a, size_t width_a, size_t height_a,
    size_t x_b, size_t y_b, size_t width_b, size_t height_b)
{
    if (x_a < x_b + width_b && x_a + width_a > x_b &&
        y_a < y_b + height_b && y_a + height_a > y_b)
        return true;
    return false;
}


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
                      size_t death_width, size_t *score)
{
    for (size_t bi = 0; bi < game->num_bullets;)
    {
        bool bullet_destroyed = false;
        
        for (size_t ai = 0; ai < game->num_aliens; ++ai)
        {
            Alien &alien = game->aliens[ai];
            if (alien.type == ALIEN_DEAD) continue;

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