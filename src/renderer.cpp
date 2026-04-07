#include "renderer.h"
#include "common.h"
#include <cstdio>
#include <GL/glew.h>

// #define GL_ERROR_CASE(glerror) \
//     case glerror:              \
//         snprintf(error, sizeof(error), "%s", #glerror)
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

// #undef GL_ERROR_CASE

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8) | 255;
}

void buffer_clear(Buffer *buffer, uint32_t color)
{
    for (size_t i = 0; i < buffer->width * buffer->height; ++i)
    {
        buffer->data[i] = color;
    }
}

void buffer_draw_sprite(
    Buffer *buffer, const Sprite &sprite, size_t x, size_t y, uint32_t color)
{
    for (size_t xi = 0; xi < sprite.width; ++xi)
    {
        for (size_t yi = 0; yi < sprite.height; ++yi)
        {
            size_t sy = sprite.height - 1 + y - yi;
            size_t sx = x + xi;
            if (sprite.data[yi * sprite.width + xi] &&
                sy < buffer->height &&
                sx < buffer->width)
            {
                buffer->data[sy * buffer->width + sx] = color;
            }
        }
    }
}

void buffer_draw_text(
    Buffer *buffer,
    const Sprite &text_spritesheet,
    const char *text,
    size_t x, size_t y,
    uint32_t color)
{
    size_t xp = x;
    size_t stride = text_spritesheet.width * text_spritesheet.height;
    Sprite sprite = text_spritesheet;
    for (const char *charp = text; *charp != '\0'; ++charp)
    {
        char character = *charp - 32;
        if (character < 0 || character >= 65)
            continue;

        sprite.data = text_spritesheet.data + character * stride;
        buffer_draw_sprite(buffer, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}

void buffer_draw_number(
    Buffer *buffer,
    const Sprite &number_spritesheet, size_t number,
    size_t x, size_t y,
    uint32_t color)
{
    uint8_t digits[64];
    size_t num_digits = 0;

    size_t current_number = number;
    do
    {
        digits[num_digits++] = current_number % 10;
        current_number = current_number / 10;
    } while (current_number > 0);

    size_t xp = x;
    size_t stride = number_spritesheet.width * number_spritesheet.height;
    Sprite sprite = number_spritesheet;
    for (size_t i = 0; i < num_digits; ++i)
    {
        uint8_t digit = digits[num_digits - i - 1];
        sprite.data = number_spritesheet.data + digit * stride;
        buffer_draw_sprite(buffer, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}

void validate_shader(GLuint shader, const char *file = 0)
{
    static const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
    if (length > 0)
    {
        printf("Shader %d(%s) compile error: %s\n", shader, (file ? file : ""), buffer);
    }
}

bool validate_program(GLuint program)
{
    static const GLsizei BUFFER_SIZE = 512;
    GLchar buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
    if (length > 0)
    {
        printf("Program %d link error: %s\n", program, buffer);
        return false;
    }
    return true;
}