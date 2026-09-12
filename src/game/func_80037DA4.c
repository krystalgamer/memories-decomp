#include "../types.h"
#include "../game/card_constants.h"
#include "../game/func_80036C14.h"
#include "../game/duel_card.h"
#include "../game/text_constants.h"
#include "../game/duel_effect.h"
#include "../game/text_encode_decimal_digits.h"
#include "../game/func_80036D70.h"
#include "../game/func_80036D3C.h"
#include "../game/duel_effect_command.h"

/* Entries 0 through 12 of the secondary text-command table D_80090EAC,
   the handlers the F8 escape reaches, together with func_80038024, the
   helper two of them share. Each takes the text channel and reads its
   operands from the channel's live stream. Entry 13, Text_StartCampaignDuel,
   is next in both the table and the image, but it only builds at
   gcc_2_8_1_g0 and stays its own unit.

   The eight former sources were recorded at gcc_2_8_1_g8_split,
   gcc_2_8_1_g8, gcc_2_8_1_g0 and gcc_2_8_1_g0_split, and every member
   compiles to an identical object at gcc_2_8_1_g8_split. Bounded below by
   the primary handlers Text_ExtendGlyphCode and Text_SetStateFromStream in
   text_stream_commands.c. */

void func_80037DA4(u8 *object)
{
    u8 **stream;
    u8 *current;
    s32 op;
    s32 id;
    s32 n;
    s32 kind;
    s32 stats;
    s32 type;
    u8 *text;
    u8 **slot;

    kind = *(s8 *)(object + 0x58);
    kind <<= 2;
    kind = (u32)object - -kind;
    stream = (u8 **)kind;
    object[0x62] = 0;
    current = *stream;
    op = current[0];
    *stream = current + 1;
    n = 0;
    if (op & 0x10) {
        object[0x54] = D_8009B320;
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
                object[0x62] = type;
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
    object[0x58]++;
    n = id;
    if (id > 0xCFFF) {
        text = (u8 *)((u32)D_801C0000 & 0xFFFF0000) + D_801C0000[id - 0xD000];
    } else if (id > 0x7FFF) {
        text = (u8 *)((u32)D_801D5800 & 0xFFFF0000) + D_801D5800[id - 0x8000];
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)((u32)D_801B0000 & 0xFFFF0000) + D_801C0000[n];
    }
store:
    slot = &((u8 **)object)[*(s8 *)(object + 0x58)];
    *slot = text;
    return;
plain:
    *(u16 *)(object + 0x34) |= 0x80;
    if ((u8)n == 0) {
        func_80036C14((DuelEffectChannel *)object, id);
    }
    *(u16 *)(object + 0x34) &= 0xFF7F;
    *(u16 *)(object + 0x38) += 0x10;
}
