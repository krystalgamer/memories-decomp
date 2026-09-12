#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/display_object.h"
#include "../game/display_projection.h"
#include "../game/screen_projection.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "../game/func_80016784.h"
#define ORDERING_TABLE_SLOT2_ARRAY
#include "../game/ordering_tables.h"

void func_80015DFC(DisplayProjectionTrackedObject *object)
{
    ProjectedPair p;
    s32 y;
    u8 *record;
    u16 x;
    u16 next_x;
    volatile s16 *scratch;

    GsSetLsMatrix(&D_800FE148);
    record = *(u8 *volatile *)&object->record;
    scratch = (volatile s16 *)0x1F8003E0;
    x = *(volatile u16 *)(record + 0x30);
    scratch[1] = 0;
    scratch[0] = x;
    record = *(u8 *volatile *)&object->record;
    next_x = *(volatile u16 *)(record + 0x34);
    scratch[2] = next_x;
    gte_ldv0((s16 *)scratch);
    gte_rtps();
    gte_stsxy(&p);
    object->screen_x = p.x - 0x1A;
    y = p.y;
    object->screen_y = y - 0x1E;
    if (D_8009B1D5 != 0) {
        if (object->field_18 < 0xF) {
            object->screen_y = y - 0x1D;
        }
    } else {
        if (object->field_18 >= 0xF) {
            object->screen_y = y - 0x1D;
        }
    }
    func_80016784(object->record, (s32)D_800E9D98[0], object->screen_x,
                  object->screen_y);
    SetGeomOffset(0xA0, 0x6C);
}
