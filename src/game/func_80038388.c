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
