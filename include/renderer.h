#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <cstddef>
#include <GL/glew.h>

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