#ifndef YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H
#define YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H

#include "../ygo_types.h"

void *Model_GetImagePrimitiveHandler(u32 value);
/* The handler it returns; callers cast the result to call it. */
typedef s32 (*ModelImagePrimitiveHandler)();
void *Model_GetPrimitiveHandler(u32 primitive_code);
void Model_BindPrimitiveHandler(ModelHandlerObject *object);
s32 Model_GetSharedPrimitiveHandler(s32 primitive_code);
void Model_BindSharedPrimitiveHandler(ModelHandlerObject *object);

#endif
