#ifndef YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H
#define YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H

#include "../ygo_types.h"

#ifdef FUNC_800603DC_RETURNS_HANDLER
s32 (*Model_GetImagePrimitiveHandler())();
#else
void *Model_GetImagePrimitiveHandler(u32 value);
#endif
void *Model_GetPrimitiveHandler(u32 arg0);
void Model_BindPrimitiveHandler(ModelHandlerObject *object);
s32 Model_GetSharedPrimitiveHandler(s32 arg0);
void Model_BindSharedPrimitiveHandler(ModelHandlerObject *object);

#endif
