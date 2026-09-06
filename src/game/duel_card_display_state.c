#include "../types.h"

#include "card_constants.h"
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
extern s32 gDuel_adwCardStats[];

u8 *func_8004002C(void);
u8 *func_800400AC(u8 *arg0, s32 arg1);
void func_80016778();
void func_80016D04();
void func_80042918(u8 *arg0);

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
    if (card->flags &
        (DUEL_CARD_FLAG_DEFENSE_POSITION | DUEL_CARD_FLAG_FACE_DOWN)) {
        object->flags = flags | 4;
        object->field_21 = 0;
        if (card->flags & DUEL_CARD_FLAG_FACE_DOWN) {
            object->field_21 = 0x80;
        }
        object->field_22 = 0;
        if (card->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
            object->field_22 = 0xC0;
        }
    }
    object->color = 0x808080;
    if (card->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        object->color = 0x404040;
    }
}

/* Creates a display object for the supplied duel card record. The caller at
 * 0x80018004 only supplies the record; arg1 and arg2 are the retail a1/a2
 * position values and intentionally remain part of this prototype. */
u8 *func_80017F04(u8 *arg0, s32 arg1, s32 arg2)
{
    u8 *p = func_800400AC(func_8004002C(), 6);
    s32 *tbl;
    s32 k;

    k = *(s16 *)(arg0 + 0xC) - 1;
    tbl = gDuel_adwCardStats;
    p[0x67] = 0;
    p[0x68] = (tbl[k] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    p[0x69] = 0;
    p[0x6A] = ((u32)arg0 - (u32)D_801A7AD8) / DUEL_CARD_RECORD_SIZE;
    p[0x6B] = (*(u8 **)(arg0 + 4))[2];
    *(s16 *)(p + 0x30) = arg1;
    *(s16 *)(p + 0x32) = arg2;
    *(s32 *)(p + 4) |= 0x1000000;
    *(void **)(p + 0x10) = func_80016778;
    func_80042918(p);
    *(void **)(p + 0x4C) = func_80016D04;
    func_80017E3C((DuelCardDisplayObject *)p);
    func_80017DB4((DuelCardDisplayObject *)p);
    return p;
}
