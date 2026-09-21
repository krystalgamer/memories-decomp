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

/* Prefix of the secondary text-command table D_80090EAC together with
   func_80038024, the helper two entries share. Each handler takes the text
   channel and reads its operands from the channel's live stream. The
   standalone func_80038110 and duel_effect_command_suffix.c continue the
   table at the same gcc_2_8_1_g8_split profile. */

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

void func_80038024(DuelEffectChannel *object, s32 value)
{
    *(u8 *)&object->flags_34 = *(u8 *)&object->flags_34;
    object->flags_34 |= 0x80;
    func_80036C14(object, value);
    object->flags_34 &= 0xFF7F;
    object->field_38 += 0x10;
}

void func_80038070(DuelEffectChannel *object)
{
    func_80038024(object, D_8009B344);
}

void func_80038094(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];

    func_80038024(object, *(*stream)++);
}
