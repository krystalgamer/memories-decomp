#include "../types.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"

extern void *func_8004006C(void);
void *func_8002E3FC(void) {
    unsigned char *object = func_800400AC((s32)func_8004006C(),2);
    func_800404CC(object,0x10,0xB0,0,0,0,0xD,0x100);
    *(unsigned short *)(object + 8) |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_80042918(object);
    return object;
}
