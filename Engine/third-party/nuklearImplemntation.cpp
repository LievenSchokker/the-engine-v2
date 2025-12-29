//These are nuklear specefic define's PLEASE DO NOT TOUCH THESE
//REMOVING OR CHANGING THIS WILL BREAK NUKLEAR.


#include <string.h>
#include <math.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_IO

#define NK_MEMSET memset
#define NK_MEMCPY memcpy
#define NK_SQRT sqrt
#define NK_SIN sinf
#define NK_COS cosf

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear_sdl_renderer.h"