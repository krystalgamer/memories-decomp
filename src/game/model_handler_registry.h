#ifndef YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H
#define YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H

#include "../types.h"

void Model_RegisterHandlerKey(s32 key, s32 val);
s32 Model_FindHandlerKey(s32 val);
void func_80060220(s32 arg0, u8 *arg1, u8 *arg2);
void func_80069E44(void);
void func_80069F94(void);
void func_8006A0E8(void);
void func_8006A268(void);
void func_8006A3F0(void);
void func_8006A540(void);
void func_8006A694(void);
void func_8006A814(void);
void func_8006A99C(void);
void func_8006AAFC(void);
void func_8006AC88(void);
void func_8006ADE8(void);

/* Open-codes the same reverse lookup as Model_FindHandlerKey -- same
 * sentinel, same eighty-entry scan of D_800F5918, same -1 -- and dispatches on
 * the packed id it returns. func_80059DD8 in model_scene_states.c is the only
 * outside consumer and takes its address as an s32 (*)(s32) callback rather
 * than calling it, so this declaration has to keep that exact shape. */
s32 func_8005FC1C(s32 arg0);

#endif
