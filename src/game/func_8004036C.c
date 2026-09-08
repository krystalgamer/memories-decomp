#include "../types.h"
#include "display_object_api.h"

extern void func_8004020C(void *);

void func_8004036C(void *object)
{
    if (object != 0) {
        func_8004020C(object);
    }
}
