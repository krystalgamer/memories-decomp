#include "../types.h"

#include "duel_card.h"

typedef struct {
    u8 pad_00[4];
    u8 field_04;
} DuelCardDisplayData;

typedef struct {
    u8 pad_00[0x1F];
    s8 field_1F;
} DuelCardDisplayState;

typedef struct {
    u8 pad_00[0x08];
    u16 flags;
    u8 pad_0A[0x02];
    u32 color;
    u8 pad_10[0x11];
    u8 field_21;
    u8 field_22;
    u8 pad_23[0x44];
    u8 field_67;
    u8 pad_68[0x02];
    u8 card_index;
} DuelCardDisplayObject;

extern DuelCardDisplayState *D_8009B1C8;

void func_80017DB4(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &D_801A7AD8[object->card_index];

    if ((*(s32 *)&card->terrain_modifier & 0xA0000000) != 0xA0000000) {
        return;
    }
    if (D_8009B1C8->field_1F != 0) {
        object->field_67 = ((DuelCardDisplayData *)card->data)->field_04 + 1;
    }
    if (D_8009B1C8->field_1F < 0) {
        object->field_67 = 0xFF;
    }
}

void func_80017E3C(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &D_801A7AD8[object->card_index];
    u16 flags;

    if (!(card->flags & 0x2000)) {
        object->field_67 = 0;
    }
    flags = object->flags & 0xFFFB;
    object->flags = flags;
    if (card->flags & 0x1800) {
        object->flags = flags | 4;
        object->field_21 = 0;
        if (card->flags & 0x1000) {
            object->field_21 = 0x80;
        }
        object->field_22 = 0;
        if (card->flags & 0x800) {
            object->field_22 = 0xC0;
        }
    }
    object->color = 0x808080;
    if (card->flags & 0x4000) {
        object->color = 0x404040;
    }
}
