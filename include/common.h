#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdint>

// --- Constants ---
#define GAME_MAX_BULLETS 128

// --- Enumerations ---
enum AlienType : uint8_t
{
    ALIEN_DEAD = 0,
    ALIEN_TYPE_A = 1,
    ALIEN_TYPE_B = 2,
    ALIEN_TYPE_C = 3
};

// --- Macros ---
// Helper for mapping GL errors to strings in gl_debug
#define GL_ERROR_CASE(glerror) \
    case glerror:              \
        snprintf(error, sizeof(error), "%s", #glerror)

#endif