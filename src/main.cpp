#include <cstdio>
#include <cstdint>
#include <cstdlib>
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

// key_callback and error_callback here.
void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        game_running = false;

    if (key == GLFW_KEY_RIGHT)
    {
        if (action == GLFW_PRESS)
            move_dir += 1;
        else if (action == GLFW_RELEASE)
            move_dir -= 1;
    }
    if (key == GLFW_KEY_LEFT)
    {
        if (action == GLFW_PRESS)
            move_dir -= 1;
        else if (action == GLFW_RELEASE)
            move_dir += 1;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        fire_pressed = true;
}

int main(int argc, char *argv[])
{
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;

    // 1. Initialization
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return -1;

    // ... (Set window hints for OpenGL 3.3 Core Profile)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(buffer_width, buffer_height, "Space Invaders", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error initializing GLEW.\n");
        glfwTerminate();
        return -1;
    }

    int glVersion[2] = {-1, 1};
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

    gl_debug(__FILE__, __LINE__);

    // printf("Using OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
    // printf("Renderer used: %s\n", glGetString(GL_RENDERER));
    // printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glfwSwapInterval(1);
    glClearColor(1.0, 0.0, 0.0, 1.0);
    uint32_t clear_color = rgb_to_uint32(0, 128, 0);

    // 2. Buffer & Renderer Setup
    Buffer buffer;
    buffer.width = buffer_width;
    buffer.height = buffer_height;
    buffer.data = new uint32_t[buffer.width * buffer.height];

    // How to make sure that it is linked properly?
    buffer_clear(&buffer, 0);

    //: Initialize textures, VAOs, and Shaders using renderer.h utilities.
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, buffer.width, buffer.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint fullscreen_triangle_vao;
    glGenVertexArrays(1, &fullscreen_triangle_vao);

    // Check if this is okay
    const char *vertex_shader = load_shader_source("./shaders/vertex_shader.glsl");
    const char *fragment_shader = load_shader_source("./shaders/fragment_shader.glsl");

    GLuint shader_id = glCreateProgram();

    // create vertex shader
    {
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader_vp, 1, &vertex_shader, 0);
        glCompileShader(shader_vp);
        validate_shader(shader_vp, vertex_shader);
        glAttachShader(shader_id, shader_vp);

        glDeleteShader(shader_vp);
    }

    // create fragment shader
    {
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader_fp, 1, &fragment_shader, 0);
        glCompileShader(shader_fp);
        validate_shader(shader_fp, fragment_shader);
        glAttachShader(shader_id, shader_fp);

        glDeleteShader(shader_fp);
    }

    glLinkProgram(shader_id);
    free((void *)vertex_shader);
    free((void *)fragment_shader);

    if (!validate_program(shader_id))
    {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();
        glDeleteVertexArrays(1, &fullscreen_triangle_vao);
        delete[] buffer.data;
        return -1;
    }

    glUseProgram(shader_id);

    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(fullscreen_triangle_vao);

    // 3. Asset Mapping (Static constants from sprites.cpp)
    // Initialize Assets from sprites.cpp
    Sprite text_spritesheet = {5, 7, (uint8_t *)TEXT_SPRITESHEET_DATA};
    Sprite number_spritesheet = {5, 7, (uint8_t *)TEXT_SPRITESHEET_DATA + (16 * 35)};
    Sprite player_sprite = {11, 7, (uint8_t *)PLAYER_SPRITE_DATA};
    Sprite bullet_sprite = {1, 3, (uint8_t *)BULLET_SPRITE_DATA};
    Sprite alien_death_sprite = {13, 7, (uint8_t *)ALIEN_DEATH_SPRITE_DATA};

    // Alien Animation Frames
    Sprite alien_sprites[6];
    alien_sprites[0] = {8, 8, (uint8_t *)ALIEN_SPRITE_0_DATA};
    alien_sprites[1] = {8, 8, (uint8_t *)ALIEN_SPRITE_1_DATA};
    alien_sprites[2] = {11, 8, (uint8_t *)ALIEN_SPRITE_2_DATA};
    alien_sprites[3] = {11, 8, (uint8_t *)ALIEN_SPRITE_3_DATA};
    alien_sprites[4] = {12, 8, (uint8_t *)ALIEN_SPRITE_4_DATA};
    alien_sprites[5] = {12, 8, (uint8_t *)ALIEN_SPRITE_5_DATA};

    SpriteAnimation alien_animation[3];
    for (int i = 0; i < 3; ++i)
    {
        alien_animation[i] = {true, 2, 10, 0, new Sprite *[2]};
        alien_animation[i].frames[0] = &alien_sprites[2 * i];
        alien_animation[i].frames[1] = &alien_sprites[2 * i + 1];
    }

    // Initialize Game State
    Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.num_bullets = 0;
    game.num_aliens = 55;
    game.aliens = new Alien[game.num_aliens];
    game.player = {112 - 5, 32, 3};

    //: Initialize alien positions in a grid.
    for (size_t yi = 0; yi < 5; ++yi)
    {
        for (size_t xi = 0; xi < 11; ++xi)
        {
            Alien &alien = game.aliens[yi * 11 + xi];
            alien.type = (5 - yi) / 2 + 1;

            // const Sprite &sprite = alien_sprites[2 * (alien.type - 1)];

            alien.x = 16 * xi + 20;
            alien.y = 17 * yi + 128;
        }
    }

    uint8_t *death_counters = new uint8_t[game.num_aliens];
    for (size_t i = 0; i < game.num_aliens; ++i)
        death_counters[i] = 10;

    game_running = true;
    int player_move_dir = 0;

    // --- Main Loop ---
    while (!glfwWindowShouldClose(window) && game_running)
    {

        buffer_clear(&buffer, clear_color);

        buffer_draw_text(
            &buffer,
            text_spritesheet, "SCORE",
            4, game.height - text_spritesheet.height - 7,
            rgb_to_uint32(128, 0, 0));

        buffer_draw_number(
            &buffer,
            number_spritesheet, score,
            4 + 2 * number_spritesheet.width, game.height - 2 * number_spritesheet.height - 12,
            rgb_to_uint32(128, 0, 0));

        buffer_draw_text(
            &buffer,
            text_spritesheet, "CREDIT 00",
            164, 7,
            rgb_to_uint32(128, 0, 0));

        // 1. SIMULATION (Using game.cpp functions)
        update_player(&game, move_dir, player_sprite.width);
        update_bullets(&game, bullet_sprite.height);

        // Pass relevant widths for collision math
        check_collisions(&game, bullet_sprite.width, bullet_sprite.height,
                         11, 8, alien_death_sprite.width, &score);

        update_aliens(&game, death_counters);

        if (fire_pressed && game.num_bullets < GAME_MAX_BULLETS)
        {
            game.bullets[game.num_bullets] = {game.player.x + player_sprite.width / 2, game.player.y + player_sprite.height, 2};
            game.num_bullets++;
            fire_pressed = false;
        }

        // 2. RENDERING
        // Entities
        for (size_t ai = 0; ai < game.num_aliens; ++ai)
        {
            if (!death_counters[ai])
                continue;
            const Alien &alien = game.aliens[ai];
            if (alien.type == ALIEN_DEAD)
            {
                buffer_draw_sprite(&buffer, alien_death_sprite, alien.x, alien.y, rgb_to_uint32(255, 0, 0));
            }
            else
            {
                SpriteAnimation &anim = alien_animation[alien.type - 1];
                buffer_draw_sprite(&buffer, *anim.frames[anim.time / anim.frame_duration], alien.x, alien.y, rgb_to_uint32(255, 255, 255));
            }
        }

        for (int i = 0; i < 3; ++i)
        {
            alien_animation[i].time = (alien_animation[i].time + 1) %
                                      (alien_animation[i].num_frames * alien_animation[i].frame_duration);
        }

        for (size_t bi = 0; bi < game.num_bullets; ++bi)
        {
            buffer_draw_sprite(&buffer, bullet_sprite, game.bullets[bi].x, game.bullets[bi].y, rgb_to_uint32(255, 255, 0));
        }

        buffer_draw_sprite(&buffer, player_sprite, game.player.x, game.player.y, rgb_to_uint32(0, 255, 0));

        // 3. DISPLAY
        // Line of separation
        for (size_t i = 0; i < game.width; ++i)
        {
            buffer.data[game.width * 16 + i] = rgb_to_uint32(128, 0, 0);
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer.width, buffer.height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    delete[] buffer.data;
    delete[] game.aliens;
    delete[] death_counters;
    for (int i = 0; i < 3; ++i)
        delete[] alien_animation[i].frames;

    glfwDestroyWindow(window);
    glfwTerminate();
    glDeleteVertexArrays(1, &fullscreen_triangle_vao);
    return 0;
}