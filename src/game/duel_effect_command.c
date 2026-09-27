#include "../types.h"
#include "card_constants.h"
#include "func_80036C14.h"
#include "duel_card.h"
#include "text_constants.h"
#include "duel_effect.h"
#include "text_encode_decimal_digits.h"
#include "text_stream_read_u32_le.h"
#include "text_stream_read_u16_le.h"
#include "duel_effect_command.h"
#include "../unmatched.h"

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))
#ifndef DUEL_EFFECT_U16_RESULT_OFFSET
#define DUEL_EFFECT_U16_RESULT_OFFSET 0x60
#endif

/* Entries 0 through 12 of the secondary text-command table tent_SecondaryTextCommandTable,
   the handlers the F8 escape reaches, together with func_80038024, the
   helper two of them share -- which is not itself a table entry, so this is
   fourteen definitions rather than thirteen entries. Each entry takes the
   text channel and reads its operands from the channel's live stream. Entry
   13, Text_StartCampaignDuel, is next in both the table and the image, but it
   only builds at gcc_2_8_1_g0 and stays its own unit.

   The nine former sources were recorded at gcc_2_8_1_g8_split,
   gcc_2_8_1_g8, gcc_2_8_1_g0 and gcc_2_8_1_g0_split, and every member
   compiles to an identical object at gcc_2_8_1_g8_split. Bounded below by
   the primary handlers Text_ExtendGlyphCode and Text_SetStateFromStream in
   text_stream_commands.c. */

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80037DA4)
#ifdef VERSION_JAPAN
/* The Japanese card-text command: no field_62 (the Japanese channel record
   ends at 0x60), a +0x100 rather than +0xD100 id for op bit 6, the stars
   without the US type and 0x17 tests, an unconditional glyph for the plain
   case, and the two-bank string lookup of the Japanese Text_LookupString. */
void func_80037DA4(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    s32 n;
    u8 *text;
    u8 *current;

    text = (u8 *)(s32)object->stream_58;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    if (op & 0x10) {
        object->field_54 = D_8009B320;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0x100;
    } else {
        id = 0;
        switch (op & 0xF) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_1_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_58++;
    n = id;
    if (id & TEXT_GLOBAL_STRING_ID_BASE) {
        text = (u8 *)(((u32)D_801D6000 & TEXT_BANK_ADDRESS_MASK) |
                      D_801D6000[id & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)(((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) |
                      D_801C0000[n]);
    }
    TEXT_STREAM_OWNER(object)->streams[object->stream_58] = text;
    return;
plain:
    object->flags_34 |= 0x80;
    func_80036C14(object, id);
    object->flags_34 &= 0xFF7F;
    object->field_38 += 0x10;
}
#elif defined(VERSION_EUROPE)
/* The European card-text command: the Japanese shape (no field_62 reset,
   +0x100 for op bit 6, the stars without the US type and 0x17 tests), a
   fourth selector (the card type with bit 7 set), the European string banks
   and the plain glyph flagged 0x100 rather than 0x80. */
void func_80037DA4(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    u8 *text;
    u8 *current;

    text = (u8 *)(s32)object->stream_58;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    if (op & 0x10) {
        object->field_54 = D_8009B320;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0x100;
    } else {
        id = 0;
        switch (op & 0xF) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_1_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 3:
            id = ((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                   CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) | 0x80;
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_58++;
    if (id <= 0x7FFF) {
        if (id >= 0x500) {
            s32 n = id - 0x100;

            text = (u8 *)(((u32)D_801C0004 & TEXT_BANK_ADDRESS_MASK) +
                          D_801B0004[n]);
        } else {
            text = (u8 *)(((u32)D_801B0004 & TEXT_BANK_ADDRESS_MASK) +
                          D_801B0004[id]);
        }
    } else {
        text = (u8 *)(((u32)D_801D5804 & TEXT_BANK_ADDRESS_MASK) +
                      D_801D5804[id - 0x8000]);
    }
    TEXT_STREAM_OWNER(object)->streams[object->stream_58] = text;
    return;
plain:
    object->flags_34 |= 0x100;
    func_80036C14(object, id);
    object->flags_34 &= 0xFEFF;
    object->field_38 += 0x10;
}
#else
void func_80037DA4(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    s32 n;
    s32 kind;
    s32 stats;
    s32 type;
    u8 *text;
    u8 *current;
    u8 **slot;

    text = (u8 *)(s32)object->stream_58;
    object->field_62 = 0;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    n = 0;
    if (op & 0x10) {
        object->field_54 = D_8009B320;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0xD100;
    } else {
        kind = op & 0xF;
        id = 0;
        switch (kind) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            stats = gDuel_adwCardStats[gDuel_wSelectedCardID - 1];
            id = (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            type = (stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            id += 0x17;
            if ((u32)(type - CARD_TYPE_MAGIC) < CARD_NON_MONSTER_TYPE_COUNT) {
                object->field_62 = type;
            }
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            if (id == 0x17) {
                n = 1;
            }
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_58++;
    n = id;
    if (id > 0xCFFF) {
        text = (u8 *)((u32)D_801C0000 & 0xFFFF0000) +
               D_801C0000[id - 0xD000];
    } else if (id > 0x7FFF) {
        text = (u8 *)((u32)D_801D5800 & 0xFFFF0000) +
               D_801D5800[id - 0x8000];
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)((u32)D_801B0000 & 0xFFFF0000) + D_801C0000[n];
    }
store:
    slot = &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    *slot = text;
    return;
plain:
    object->flags_34 |= 0x80;
    if ((u8)n == 0) {
        func_80036C14(object, id);
    }
    object->flags_34 &= 0xFF7F;
    object->field_38 += 0x10;
}
#endif
#endif

/* The channel flag bit func_80038024 holds across its call; 0x100 in the
 * European build, whose wrapper defines it. */
#ifndef DUEL_EFFECT_SELECTOR_FLAG
#define DUEL_EFFECT_SELECTOR_FLAG 0x80
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80038024)
void func_80038024(DuelEffectChannel *object, s32 value)
{
    *(u8 *)&object->flags_34 = *(u8 *)&object->flags_34;
    object->flags_34 |= DUEL_EFFECT_SELECTOR_FLAG;
    func_80036C14(object, value);
    object->flags_34 &= 0xFFFF ^ DUEL_EFFECT_SELECTOR_FLAG;
    object->field_38 += 0x10;
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_FORWARD_SELECTOR)
void func_80038070(DuelEffectChannel *object)
{
    func_80038024(object, D_8009B344);
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_STREAM_SELECTOR)
void func_80038094(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];

    func_80038024(object, *(*stream)++);
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_ADD_SIGNED_BYTE)
void func_800380D4(DuelEffectChannel *object)
{
    register u8 **stream;
    register u8 *current;
    register u32 value;

    object->field_38 = 0;
    stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    current = *stream;
    value = current[0];
    current++;
    *stream = current;
    object->field_3A += (s8)value;
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_ADD_STREAM_BYTE)
void func_80038110(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    register u8 **slot = stream;
    register u8 *current = *slot;
    register u32 value = current[0];

    current++;
    *slot = current;
    object->field_38 += value;
}
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80038148)
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
#endif

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

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_SCALED_OPERAND)
void func_800382A8(DuelEffectChannel *object)
{
#ifdef VERSION_EUROPE
    u8 value;
#else
    u32 value;
#endif

#ifdef VERSION_EUROPE
    object->flags_34 &= 0xFFFC;
#else
    object->flags_34 &= 0xFEFF;
#endif
    value = read_operand(object);
#ifdef VERSION_JAPAN
    object->field_5B = value << 3;
    object->field_5A = value << 3;
#elif defined(VERSION_EUROPE)
    /* The European release has four text sizes and keeps the size in the
       channel's two low flag bits. */
    switch (value) {
    case 0:
        object->field_5A = 8;
        object->field_5B = 16;
        break;
    case 1:
        object->field_5A = 8;
        object->field_5B = 8;
        object->flags_34 |= 1;
        break;
    case 2:
        object->field_5A = 12;
        object->field_5B = 16;
        object->flags_34 |= 2;
        break;
    case 3:
        object->field_5A = 16;
        object->field_5B = 16;
        object->flags_34 |= 3;
        break;
    }
#else
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
#endif
#ifndef VERSION_EUROPE
    if (value == 1)
        object->flags_34 |= 0x100;
#endif
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_READ_OPERAND_PAIR)
void func_80038334(DuelEffectChannel *object)
{
    /* Separate lifetimes preserve allocation across the two stream reads. */
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->field_5A = value;
    }
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->field_5B = value;
    }
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_READ_U16)
void func_80038388(DuelEffectChannel *object)
{
    object->field_38 = TextStream_ReadU16LE(object);
}
#endif

#if defined(VERSION_EUROPE) && defined(VERSION_EUROPE_FUNC_80038168)
/* A European-only command, next to func_80038388 in the image: the same
   read into field_3A. */
void func_80038168(DuelEffectChannel *object)
{
    object->field_3A = TextStream_ReadU16LE(object);
}
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_800383B0)
void func_800383B0(DuelEffectChannel *object)
{
#ifdef VERSION_EUROPE
    /* The European build clears the glyph counter and sets the limit
       (halfwords at 0x60 and 0x62) from one operand byte, times eight. */
    *(u16 *)((u8 *)object + DUEL_EFFECT_U16_RESULT_OFFSET) = 0;
    *(u16 *)((u8 *)object + DUEL_EFFECT_U16_RESULT_OFFSET + 2) =
        read_operand(object) << 3;
#else
    ((u8 *)object)[DUEL_EFFECT_U16_RESULT_OFFSET] = 0;
    ((u8 *)object)[DUEL_EFFECT_U16_RESULT_OFFSET + 1] =
        TextStream_ReadU16LE(object);
#endif
}
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_800383DC)
u32 *func_800383DC(DuelEffectChannel *a0) {
    DuelEffectChannel *a3 = a0;
    s32 a2 = D_8009B32E;
    u32 v1;
    u8 counter;
    u32 *slot;

#ifdef VERSION_JAPAN
    if (a2 & TEXT_GLOBAL_STRING_ID_BASE) {
        v1 = ((u32)D_801D6000 & TEXT_BANK_ADDRESS_MASK) |
             D_801D6000[a2 & (TEXT_GLOBAL_STRING_ID_BASE - 1)];
    } else {
        if (a2 >= 0x500) {
            a2 -= 0x100;
        }
        v1 = ((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) | D_801C0000[a2];
    }
#elif defined(VERSION_EUROPE)
    /* The European banks, as in TextBox_BuildStep. */
    if (a2 <= 0x7FFF) {
        if (a2 >= 0x500) {
            s32 n = a2 - 0x100;

            v1 = ((u32)D_801C0004 & TEXT_BANK_ADDRESS_MASK) + D_801B0004[n];
        } else {
            v1 = ((u32)D_801B0004 & TEXT_BANK_ADDRESS_MASK) + D_801B0004[a2];
        }
    } else {
        v1 = ((u32)D_801D5804 & TEXT_BANK_ADDRESS_MASK) +
             D_801D5804[a2 - 0x8000];
    }
#else
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
#endif

    counter = *(u8 *)&a3->stream_58 + 1;
    *(u8 *)&a3->stream_58 = counter;
    slot = (u32 *)&TEXT_STREAM_OWNER(a3)->streams[(s8)counter];
    *slot = v1;
    return slot;
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_SET_COLOR_SLOT)
void func_80038498(DuelEffectChannel *object)
{
    u8 **slot =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    u8 *q = *slot;
    s32 v = *q;
    s32 w;

    *slot = q + 1;
    w = v;
    if (v & 0x80) {
        w = gText_abColorSlots[v & 0xF];
    }
    object->field_54 = w;
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_DUEL_EFFECT_SET_FLAG_1000)
void func_800384E4(DuelEffectChannel*object){register DuelEffectChannel*obj;register u8**stream;register u8*current;register unsigned int value;obj=object;obj->flags_34&=0xEFFF;stream=&((u8**)obj)[obj->stream_58];current=*stream;value=*current;current++;*stream=current;if(value)obj->flags_34|=0x1000;}
#endif
