#ifndef YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H
#define YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H

#include "../types.h"

void Model_RegisterHandlerKey(s32 key, s32 val);
s32 Model_FindHandlerKey(s32 val);
void func_80060220(s32 arg0, u8 *arg1, u8 *arg2);

#endif
