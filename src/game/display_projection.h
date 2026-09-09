#ifndef MEMORIES_DECOMP_DISPLAY_PROJECTION_H
#define MEMORIES_DECOMP_DISPLAY_PROJECTION_H

#include "display_object.h"

#define DISPLAY_PROJECTION_TRACKED_OFFSET(member) \
    ((u32)&(((DisplayProjectionTrackedObject *)0)->member))

/* Object tracked by func_80015DFC. `record` supplies the world coordinates
 * projected into screen_x/screen_y; field_18 selects the one-pixel y bias. */
typedef struct {
    DisplayObject *record;
    u8 pad_04[4];
    s16 screen_x;
    s16 screen_y;
    u8 pad_0C[0xC];
    s8 field_18;
    u8 pad_19[3];
} DisplayProjectionTrackedObject;

typedef char DisplayProjectionTrackedObject_screen_x_offset_must_be_0x8[
    DISPLAY_PROJECTION_TRACKED_OFFSET(screen_x) == 0x8 ? 1 : -1
];
typedef char DisplayProjectionTrackedObject_field_18_offset_must_be_0x18[
    DISPLAY_PROJECTION_TRACKED_OFFSET(field_18) == 0x18 ? 1 : -1
];
typedef char DisplayProjectionTrackedObject_size_must_be_0x1C[
    sizeof(DisplayProjectionTrackedObject) == 0x1C ? 1 : -1
];

#undef DISPLAY_PROJECTION_TRACKED_OFFSET

void func_80015D18(DisplayObject *object);
void func_80015DB8(DisplayObject *object);
void func_80015DFC(DisplayProjectionTrackedObject *object);

#endif
