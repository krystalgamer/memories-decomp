/* The two contiguous display-object ground projections: DisplayObject_ProjectGroundPosition
 * projects an object's ground position into its screen fields, and
 * DisplayObject_ProjectLinkedGroundPosition copies the position of the pool
 * object the target links to before calling it.
 *
 * Scratchpad C stores and official Psy-Q GTE macros reproduce the projection
 * block. The compiler profile normalizes Psy-Q's RTPS placeholder opcode. */
#include "../types.h"
#include "scratchpad.h"
#include "duel_scene.h"
#include "display_object.h"
#include "display_projection.h"
#include "model.h"
#include "model_slot_support.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "ordering_tables.h"

void DisplayObject_ProjectGroundPosition(DisplayObject *object)
{
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    {
        SVECTOR *scratch = (SVECTOR *)SCRATCH_ADDR(0x3E0);
        u16 x = object->position.h.field_28;

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz = object->position.h.field_2A;
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(&object->xy);
    }
    object->xy.h.x -= 0x20;
    object->xy.h.y -= 0x1E;
    SetGeomOffset(0, 0);
}

void DisplayObject_ProjectLinkedGroundPosition(DisplayObject *object)
{
    DisplayObject *entry;

    entry = gDisplayObject_aPool + object->field_6A;
    object->position.word = entry->position.word;
    DisplayObject_ProjectGroundPosition(object);
}
