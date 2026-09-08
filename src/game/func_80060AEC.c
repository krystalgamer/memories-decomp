#include "../types.h"
#include "func_800608B8.h"
#include "func_80060AEC.h"
#include "model_handler_registry.h"

void func_80060AEC(ModelHandlerObject *object)
{
    *object->handler = (void *)func_800608B8(object->key);
    Model_RegisterHandlerKey(object->key, (int)*object->handler);
}
