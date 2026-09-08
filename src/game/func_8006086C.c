#include "../types.h"
#include "func_8006086C.h"
#include "model_handler_registry.h"
#include "model_primitive_handler.h"

void func_8006086C(ModelHandlerObject *object)
{
    *object->handler = Model_GetPrimitiveHandler(object->key);
    Model_RegisterHandlerKey(object->key, (int)*object->handler);
}
