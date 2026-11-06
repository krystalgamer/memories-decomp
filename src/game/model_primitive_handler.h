#ifndef YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H
#define YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H

#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* Returns the libhmd image-primitive driver for an IMAGE primitive type. */
u32 *(*Model_GetImagePrimitiveHandler(u32 value))(GsARGUNIT *);
void *Model_GetPrimitiveHandler(u32 primitive_code);
void Model_BindPrimitiveHandler(ModelHandlerObject *object);
s32 Model_GetSharedPrimitiveHandler(s32 primitive_code);
void Model_BindSharedPrimitiveHandler(ModelHandlerObject *object);

#endif
