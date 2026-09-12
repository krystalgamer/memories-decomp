#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_LIST_RENDERERS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_LIST_RENDERERS_H

#include "../types.h"
#include "sprite_primitive.h"

/* The untextured and textured scratchpad renderers. */
void func_80040DD8(void);
void func_80041068(void);

struct Func80028B08Ctx;
struct Func80028B08Extra;

#ifdef FUNC_80042188_RAW_ATTRIBUTE_VIEW
#define FUNC_80042188_ARG0 s32
#define FUNC_80042188_ARG1 u8 *
#define FUNC_80042188_ARG4 u8 *
#elif defined(FUNC_80042188_CARD_LIST_VIEW)
#define FUNC_80042188_ARG0 SpritePrim *
#define FUNC_80042188_ARG1 struct Func80028B08Ctx *
#define FUNC_80042188_ARG4 struct Func80028B08Extra *
#endif

#ifdef FUNC_80042188_ARG0
void func_80042188(FUNC_80042188_ARG0 arg0, FUNC_80042188_ARG1 arg1,
                   s32 arg2, s32 arg3, FUNC_80042188_ARG4 arg4);
#undef FUNC_80042188_ARG0
#undef FUNC_80042188_ARG1
#undef FUNC_80042188_ARG4
#endif

#endif
