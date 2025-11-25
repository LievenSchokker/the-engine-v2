// Standard library includes MUST come before nuklear.h
#include <string.h>  // for memset, memcpy
#include <math.h>    // for sin, cos, sqrt

// Nuklear configuration defines
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

// Point Nuklear to the standard functions
#define NK_MEMSET memset
#define NK_MEMCPY memcpy
#define NK_SQRT sqrt
#define NK_SIN sinf
#define NK_COS cosf

// NOW include nuklear implementation
#define NK_IMPLEMENTATION
#include "nuklear.h"

// SDL renderer implementation
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear_sdl_renderer.h"