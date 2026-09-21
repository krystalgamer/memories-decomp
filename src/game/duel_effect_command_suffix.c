#include "../types.h"
#include "text_constants.h"
#include "duel_effect.h"
#include "text_encode_decimal_digits.h"
#include "text_stream_read_u32_le.h"
#include "text_stream_read_u16_le.h"
#include "../unmatched.h"

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

void func_80038148(DuelEffectChannel *object)
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

    r = TextStream_ReadU32LE(TEXT_STREAM_OWNER(object));
    t = *TEXT_STREAM_OWNER(object)->streams[object->stream_58]++;
    c = t;
    Text_EncodeDecimalDigits(*(s32 *)r, c & 0xF, buf);

    h = 0;

    if ((c & 0x80) != 0) {
        if ((c & 0x40) == 0) {
            goto skip;
        }
        h = *(u16 *)&D_800EAFF8[0];
        e = object->text_44;
        goto write;
    }

    if (c < 2) {
        e = object->text_44;
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
    e = object->text_44;

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
    object->stream_58++;
    TEXT_STREAM_OWNER(object)->streams[object->stream_58] = object->text_44;
}

/* Inlining keeps the stream value and channel in independent live ranges. */
static __inline__ u32 read_operand(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    u8 *cursor = *stream;
    u32 value = *cursor++;

    *stream = cursor;
    return value;
}

void func_800382A8(DuelEffectChannel *object)
{
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
