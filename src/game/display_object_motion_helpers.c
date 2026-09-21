#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "gpu_packets.h"
#include "display_object_config.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "display_object_lifecycle.h"
#include "display_object_packet_submit.h"

/* Scratchpad work areas: the four quad vertices at 0x1F800300, the
 * RotAverageNclip4 depth/flag results at 0x1F8002E0, and the DivideFT4
 * inputs - its DIVPOLYGON4 at 0x1F800000, the colour at 0x1F800280 and the
 * four texture coordinates from 0x1F800290. */
#define SCRATCH_VERTEX(i) ((SVECTOR *)0x1F800300 + (i))
#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))
#define GS_OT_VIEW(ordering_table) ((GsOT *)(ordering_table))
#define POLY_G4_VIEW(packet) ((POLY_G4 *)(packet))
#define POLY_GT4_VIEW(packet) ((POLY_GT4 *)(packet))
#define POLY_FT4_VIEW(packet) ((POLY_FT4 *)(packet))

/* The high half of `mode` selects the handler and the low half is the
 * ordering-table depth. Cases 1-3 hand a GsSPRITE to libgs. Cases 4 and 5 get
 * the display object's attribute word in place of the sprite pointer and
 * submit a prepared POLY_G4 or POLY_GT4, projecting it first when GsPERS
 * (0x04000000) is set. Any other case builds a POLY_FT4 from the sprite and
 * projects it, subdividing through DivideFT4 when `extra` asks for it. */
void DisplayObject_StepPositionXY(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
}

void DisplayObject_StepPositionXYZ(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
    DisplayObject_StepPositionZ(object);
}

s32 DisplayObject_StepToward(s32 value, s32 target, s32 step)
{
    if (target < 0) {
        value -= step;
        if (value < target) {
            value = target;
        }
    } else {
        value += step;
        if (value > target) {
            value = target;
        }
    }
    return value;
}

s32 DisplayObject_StepTowardZero(s32 value, s32 step)
{
    if (value < 0) {
        value += step;
        if (value > 0) {
            value = 0;
        }
    } else {
        value -= step;
        if (value < 0) {
            value = 0;
        }
    }
    return value;
}

#define DISPLAY_OBJECT_FROM_FIELD_6C(field) \
    ((DisplayObject *)((field) - 0x6C))

void *DisplayObject_FindAllocatedByTag(s32 value)
{
    u8 *object = (u8 *)D_800EFE48;
    s32 count = DISPLAY_OBJECT_POOL_CAPACITY;
    u8 *field = (u8 *)&((DisplayObject *)object)->field_6C;

    do {
        if ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->flags &
             DISPLAY_OBJECT_FLAG_ALLOCATED) &&
            ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->field_6C & 0xF) == value))
            return object;
        field += DISPLAY_OBJECT_RECORD_SIZE;
        count--;
        object += DISPLAY_OBJECT_RECORD_SIZE;
    } while (count != 0);
    return 0;
}

s32 DisplayObject_MarkInitialized(DisplayObjectLifecycle *object)
{
    if ((object->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_ALLOCATED;
        return 0;
    }
    return 1;
}

void DisplayObject_FadeBrightnessAndRelease(DisplayObjectLifecycle *object)
{
    s32 value = object->red - object->fade_step;

    if (value > 0) {
        object->blue = value;
        object->green = value;
        object->red = value;
    } else {
        DisplayObject_ReleaseIfPresent(object);
    }
}
