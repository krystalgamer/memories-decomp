#include "../types.h"
#include "../psyq/libapi.h"
#include "func_800440B4.h"
#include "io_event_helpers.h"

extern long gMemCard_aIOEventHandles[];

int func_800440F0(int value)
{
    int result;
    if (func_800440B4(value, 1)) {
        func_80043D48(gMemCard_aIOEventHandles);
        _card_info(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
