#include "../types.h"
#include "card_constants.h"
#include "func_80036C14.h"
#include "duel_card.h"
#include "text_constants.h"
#include "duel_effect.h"
#include "text_encode_decimal_digits.h"
#include "func_80036D70.h"
#include "func_80036D3C.h"
#include "duel_effect_command.h"

/* Entries 1 through 12 of the secondary text-command table D_80090EAC,
   the handlers the F8 escape reaches, together with func_80038024, the
   helper two of them share -- which is not itself a table entry, so this is
   thirteen definitions rather than thirteen entries. Entry 0 is
   func_80037DA4, which stays a candidate in src/candidates/ and is not part
   of this unit. Each entry takes the text channel and reads its operands
   from the channel's live stream. Entry 13, Text_StartCampaignDuel, is next
   in both the table and the image, but it only builds at gcc_2_8_1_g0 and
   stays its own unit.

   The eight former sources were recorded at gcc_2_8_1_g8_split,
   gcc_2_8_1_g8, gcc_2_8_1_g0 and gcc_2_8_1_g0_split, and every member
   compiles to an identical object at gcc_2_8_1_g8_split. Bounded below by
   the primary handlers Text_ExtendGlyphCode and Text_SetStateFromStream in
   text_stream_commands.c. */

void func_80038024(DuelEffectChannel *object, s32 value)
{
    *(u8 *)&object->flags_34 = *(u8 *)&object->flags_34;
    object->flags_34 |= 0x80;
    func_80036C14(object, value);
    object->flags_34 &= 0xFF7F;
    object->field_38 += 0x10;
}

extern u8 D_8009B344;

void func_80038070(DuelEffectChannel *object)
{
    func_80038024(object, D_8009B344);
}

void func_80038094(DuelEffectChannel *object)
{
    u8 **stream = &((u8 **)object)[object->stream_58];

    func_80038024(object, *(*stream)++);
}

void func_800380D4(DuelEffectChannel *object)
{
    register u8 **stream;
    register u8 *current;
    register u32 value;

    object->field_38 = 0;
    stream = &((u8 **)object)[object->stream_58];
    current = *stream;
    value = current[0];
    current++;
    *stream = current;
    object->field_3A += (s8)value;
}

void func_80038110(DuelEffectChannel *object)
{
    u8 **stream = &((u8 **)object)[object->stream_58];
    register u8 **slot = stream;
    register u8 *current = *slot;
    register u32 value = current[0];

    current++;
    *slot = current;
    object->field_38 += value;
}

void func_80038148(u8 *p)
{
    u8 buf[8];
    u8 *e;
    u8 *bp;
    s32 r;
    s32 c;
    s32 t;
    s32 k;
    s32 i;
    s32 h;
    s32 w;

    r = func_80036D70(p);
    t = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
    c = t;
    Text_EncodeDecimalDigits(*(s32 *)r, c & 0xF, buf);

    h = 0;

    if ((c & 0x80) != 0) {
        if ((c & 0x40) == 0) {
            goto skip;
        }
        h = *(u16 *)&D_800EAFF8[0];
        e = p + 0x44;
        goto write;
    }

    if (c < 2) {
        e = p + 0x44;
        goto write;
    }

    bp = buf;
    k = c - 1;
    while (1) {
        if (bp[k] < TEXT_DECIMAL_RADIX) {
            break;
        }
        c = k;
        if (k < 2) {
            break;
        }
        k = c - 1;
    }

skip:
    e = p + 0x44;

write:
    i = (c & 0xF) - 1;
    do {
        w = h;
        if (buf[i] < TEXT_DECIMAL_RADIX) {
            w = *(u16 *)&D_800EAFF8[buf[i]];
        }
        if (w >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
            *e = (w >> 8) - 0x10;
            e[1] = w;
            e += 2;
        } else {
            *e = w;
            e += 1;
        }
        i--;
    } while (i >= 0);

    *e = TEXT_STRING_TERMINATOR;
    p[0x58] = p[0x58] + 1;
    *(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)) = p + 0x44;
}

/* Inlining keeps the stream value and channel in independent live ranges. */
static __inline__ u32 read_operand(DuelEffectChannel *object)
{
    u8 **stream = &((u8 **)object)[object->stream_58];
    u8 *cursor = *stream;
    u32 value = *cursor++;

    *stream = cursor;
    return value;
}

void func_800382A8(u8 *argument)
{
    DuelEffectChannel *object = (DuelEffectChannel *)argument;
    u32 value;

    object->flags_34 &= 0xFEFF;
    value = read_operand(object);
    switch (value) {
    case 1:
        object->field_5A = 8;
        object->field_5B = 8;
        break;
    case 2:
        object->field_5A = 8;
        object->field_5B = 12;
        break;
    }
    if (value == 1)
        object->flags_34 |= 0x100;
}

void func_80038334(DuelEffectChannel *object)
{
    /* Separate lifetimes preserve allocation across the two stream reads. */
    {
        u8 **stream = &((u8 **)object)[object->stream_58];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->field_5A = value;
    }
    {
        u8 **stream = &((u8 **)object)[object->stream_58];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->field_5B = value;
    }
}

void func_80038388(DuelEffectChannel *object)
{
    object->field_38 = func_80036D3C(object);
}

void func_800383B0(DuelEffectChannel *object)
{
    object->field_60 = 0;
    object->field_61 = func_80036D3C(object);
}

u32 *func_800383DC(DuelEffectChannel *a0) {
    DuelEffectChannel *a3 = a0;
    s32 a2 = D_8009B32E;
    u32 v1;
    u8 counter;
    s32 offset;
    u32 *slot;

    if (a2 > 0xCFFF) {
        v1 = ((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) +
             D_801C0000[a2 - 0xD000];
    } else if (a2 > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
        v1 = ((u32)D_801D5800 & TEXT_BANK_ADDRESS_MASK) +
             D_801D5800[a2 - TEXT_GLOBAL_STRING_ID_BASE];
    } else {
        if (a2 >= 0x500) {
            a2 -= 0x100;
        }
        v1 = ((u32)D_801B0000 & TEXT_BANK_ADDRESS_MASK) + D_801C0000[a2];
    }

    counter = *(u8 *)&a3->stream_58 + 1;
    *(u8 *)&a3->stream_58 = counter;
    offset = (s8)counter;
    slot = (u32 *)((u8 *)a3 + offset * 4);
    *slot = v1;
    return slot;
}

void func_80038498(u8 *arg0)
{
    u8 **slot = (u8 **)(arg0 + *(s8 *)(arg0 + 0x58) * 4);
    u8 *q = *slot;
    s32 v = *q;
    s32 w;

    *slot = q + 1;
    w = v;
    if (v & 0x80) {
        w = gText_abColorSlots[v & 0xF];
    }
    arg0[0x54] = w;
}

void func_800384E4(u8*object){register u8*obj;register u8**stream;register u8*current;register unsigned int value;obj=object;*(u16*)(obj+0x34)&=0xEFFF;stream=&((u8**)obj)[*(s8*)(obj+0x58)];current=*stream;value=*current;current++;*stream=current;if(value)*(u16*)(obj+0x34)|=0x1000;}
