#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <GL/glew.h>

#define GL_ERROR_CASE(glerror) \
    case glerror:              \
        snprintf(error, sizeof(error), "%s", #glerror)
inline void gl_debug(const char *file, int line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        char error[128];

        switch (err)
        {
            GL_ERROR_CASE(GL_INVALID_ENUM);
            break;
            GL_ERROR_CASE(GL_INVALID_VALUE);
            break;
            GL_ERROR_CASE(GL_INVALID_OPERATION);
            break;
            GL_ERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
            break;
            GL_ERROR_CASE(GL_OUT_OF_MEMORY);
            break;
        default:
            snprintf(error, sizeof(error), "%s", "UNKNOWN_ERROR");
            break;
        }

        fprintf(stderr, "%s - %s: %d\n", error, file, line);
    }
}

#undef GL_ERROR_CASE

// --- Core Rendering Structures ---

struct Buffer
{
    size_t width, height;
    uint32_t *data; // RGBA pixel data
};

struct Sprite
{
    size_t width, height;
    uint8_t *data; // 1-bit or alpha mask data
};

struct SpriteAnimation
{
    bool loop;
    size_t num_frames;
    size_t frame_duration;
    size_t time;
    Sprite **frames; // Pointer to an array of Sprite pointers
};

// --- Function Prototypes ---

// Utility
void gl_debug(const char *file, int line);
uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b);
void buffer_clear(Buffer *buffer, uint32_t color);

// Drawing
void buffer_draw_sprite(Buffer *buffer, const Sprite &sprite, size_t x, size_t y, uint32_t color);
void buffer_draw_text(Buffer *buffer, const Sprite &text_spritesheet, const char *text, size_t x, size_t y, uint32_t color);
void buffer_draw_number(Buffer *buffer, const Sprite &number_spritesheet, size_t number, size_t x, size_t y, uint32_t color);

// Shader Management
void validate_shader(GLuint shader, const char *file = 0);
bool validate_program(GLuint program);
char *load_shader_source(const char *path);

#endif