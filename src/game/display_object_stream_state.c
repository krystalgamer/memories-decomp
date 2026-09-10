#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/rand.h"
#include "display_object_stream_state.h"

/* The seven display-object stream opcode handlers, entries 0 through 6 of
   D_80090FEC in model_record_tables.c, which func_80041C8C dispatches as
   table[op ^ 0xFF] for opcodes 0xFF down to 0xF9: the counter resets, the
   no-op, the two jumps, the random jump, and the four-operand attribute
   command. Each reads its operands from the object's stream and returns
   whether the interpreter should keep going.

   The three former sources were recorded at gcc_2_8_1_g8 and
   gcc_2_8_1_g8_split, and every member compiles to an identical object at
   gcc_2_8_1_g8_split. Bounded below by func_80041340, the light and
   geometry setup, and above by unmatched assembly. */

s32 func_8004141C(DisplayObjectStreamState *object)
{
    object->field_5A = 0;
    return -1;
}

s32 func_80041428(DisplayObjectStreamState *object)
{
    object->field_58 = 0;
    return 1;
}

s32 func_80041434(void)
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

int func_8004149C(u8 *p, u8 *t)
{
    int i = rand() % t[0];
    unsigned hi, lo, base;

    t += i * 2 + 1;
    hi = t[1];
    lo = t[0];
    base = *(unsigned *)(p + 0x54);
    *(u16 *)(p + 0x58) = 0;
    *(unsigned *)(p + 0x50) = base + ((hi << 8) | lo);
    return 1;
}

int func_80041534(u8 *object, u8 *data)
{
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
}
