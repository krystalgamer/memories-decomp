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

void func_80015D18(DisplayObject *object)
{
    u16 value;
    u16 next_value;
    volatile s16 *scratch;

    SetGeomScreen(0x12C);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    value = *(volatile u16 *)&object->position.h.field_28;
    scratch = (volatile s16 *)0x1F8003E0;
    scratch[1] = 0;
    scratch[0] = value;
    next_value = *(volatile u16 *)&object->position.h.field_2A;
    scratch[2] = next_value;
    gte_ldv0((s16 *)scratch);
    gte_rtps();
    gte_stsxy(&object->field_30);
    object->field_30.h.field_30 -= 0x20;
    object->field_30.h.field_32 -= 0x1E;
    SetGeomOffset(0, 0);
}
