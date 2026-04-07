#include <cstdio>
#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "renderer.h"
#include "game.h"
#include "sprites.h"

// State globals
bool game_running = false;
int move_dir = 0;
bool fire_pressed = false;
size_t score = 0;

//: Include your existing key_callback and error_callback here.

int main(int argc, char *argv[]) {
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;

    // 1. Initialization
    if (!glfwInit()) return -1;
    // ... (Set window hints for OpenGL 3.3 Core Profile)
    GLFWwindow* window = glfwCreateWindow(buffer_width, buffer_height, "Space Invaders", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewInit();

    // 2. Buffer & Renderer Setup
    Buffer buffer;
    buffer.width = buffer_width;
    buffer.height = buffer_height;
    buffer.data = new uint32_t[buffer.width * buffer.height];

    //: Initialize your textures, VAOs, and Shaders using renderer.h utilities.
    

    // 3. Asset Mapping (Static constants from sprites.cpp)
    Sprite player_sprite = { 11, 7, (uint8_t*)PLAYER_SPRITE_DATA };
    Sprite bullet_sprite = { 1, 3, (uint8_t*)BULLET_SPRITE_DATA };
    Sprite alien_death_sprite = { 13, 7, (uint8_t*)ALIEN_DEATH_SPRITE_DATA };

    // 4. Game State
    Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.num_bullets = 0;
    game.num_aliens = 55;
    game.aliens = new Alien[game.num_aliens];
    game.player = { 112 - 5, 32, 3 };

    //: Initialize alien positions in a grid here.

    uint8_t *death_counters = new uint8_t[game.num_aliens];
    for (size_t i = 0; i < game.num_aliens; ++i) death_counters[i] = 10;

    game_running = true;

    while (!glfwWindowShouldClose(window) && game_running) {
        // A. Simulation (Model)
        update_player(&game, move_dir, player_sprite.width);
        update_bullets(&game, bullet_sprite.height);
        check_collisions(&game, bullet_sprite.width, bullet_sprite.height, 
                         11, 8, alien_death_sprite.width, &score);
        update_aliens(&game, death_counters);

        if (fire_pressed && game.num_bullets < GAME_MAX_BULLETS) {
            game.bullets[game.num_bullets] = { game.player.x + player_sprite.width / 2, game.player.y + player_sprite.height, 2 };
            game.num_bullets++;
            fire_pressed = false;
        }

        // B. Rendering (View)
        buffer_clear(&buffer, rgb_to_uint32(0, 128, 0));
        
        // Draw loop for Aliens
        for (size_t ai = 0; ai < game.num_aliens; ++ai) {
            if (!death_counters[ai]) continue;
            if (game.aliens[ai].type == ALIEN_DEAD) {
                buffer_draw_sprite(&buffer, alien_death_sprite, game.aliens[ai].x, game.aliens[ai].y, rgb_to_uint32(128, 0, 0));
            } else {
                // Use alien_animation logic here
            }
        }

        //: Draw bullets and player.

        // C. Display
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer.width, buffer.height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    delete[] buffer.data;
    delete[] game.aliens;
    delete[] death_counters;
    glfwTerminate();
    return 0;
}