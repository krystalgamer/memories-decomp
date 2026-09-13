#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/rand.h"
#include "display_flat_lights.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_list_renderer_table.h"
#include "display_object_stream_state.h"
#include "func_80041340.h"

void func_80041340(void)
{
    s32 i;
    SetBackColor(96, 96, 96);
    SetFarColor(0, 0, 0);
    GsSetFlatLight(0, &D_80090FCC);
    GsSetFlatLight(1, &D_80090FDC);
    GsSetFlatLight(2, &D_80090FDC);
    for (i = DISPLAY_OBJECT_LIST_COUNT - 1; i >= 0; i--) {
        if (D_800EFE38[i] >= 0) {
            SetGeomScreen(150);
            SetGeomOffset(0, 0);
            D_80090FB0[i]();
        }
    }
}

/* The seven display-object stream opcode handlers, entries 0 through 6 of
   D_80090FEC in model_record_tables.c, which func_80041C8C dispatches as
   table[op ^ 0xFF] for opcodes 0xFF down to 0xF9: the counter resets, the
   no-op, the two jumps, the random jump, and the four-operand attribute
   command. Each reads its operands from the object's stream and returns
   whether the interpreter should keep going.

   The three former sources were recorded at gcc_2_8_1_g8 and
   gcc_2_8_1_g8_split, and every member compiles to an identical object at
   gcc_2_8_1_g8_split. Together with the contiguous frame renderer above,
   this is the complete matching run between unmatched display-object code. */

s32 func_8004141C(DisplayObjectStreamState *object, const u8 *data)
{
    object->field_5A = 0;
    return -1;
}

s32 func_80041428(DisplayObjectStreamState *object, const u8 *data)
{
    object->field_58 = 0;
    return 1;
}

s32 func_80041434(DisplayObjectStreamState *object, const u8 *data)
{
    return 1;
}

s32 func_8004143C(DisplayObjectStreamState *object, const u8 *data)
{
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 func_80041464(DisplayObjectStreamState *object, const u8 *data)
{
    object->flags ^= 0x800000;
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 func_8004149C(DisplayObjectStreamState *object, const u8 *data)
{
#define p ((u8 *)object)
#define t ((u8 *)data)
    int i = rand() % t[0];
    unsigned hi, lo, base;

    t += i * 2 + 1;
    hi = t[1];
    lo = t[0];
    base = *(unsigned *)(p + 0x54);
    *(u16 *)(p + 0x58) = 0;
    *(unsigned *)(p + 0x50) = base + ((hi << 8) | lo);
    return 1;
#undef t
#undef p
}

s32 func_80041534(DisplayObjectStreamState *record, const u8 *operands)
{
#define object ((u8 *)record)
#define data ((u8 *)operands)
    int high;
    int low;
    *(u32 *)(object + 4) |= GsROTOFF;
    object[0x22] = data[0];
    *(short *)(object + 0x4A) = (signed char)data[1];
    high = data[3] << 8;
    low = data[2];
    *(u32 *)(object + 0x50) += 4;
    *(short *)(object + 0x48) = high | low;
    return 1;
#undef data
#undef object
}
