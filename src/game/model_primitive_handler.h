#ifndef YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H
#define YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H

#include "../ygo_types.h"

#ifdef FUNC_800603DC_RETURNS_HANDLER
s32 (*func_800603DC())();
#else
void *func_800603DC(u32 value);
#endif
void *Model_GetPrimitiveHandler(u32 arg0);
void func_8006086C(ModelHandlerObject *object);
s32 func_800608B8(s32 arg0);
void func_80060AEC(ModelHandlerObject *object);

#endif
