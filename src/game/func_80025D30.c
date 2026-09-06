#include "../types.h"
#include "duel_card.h"
#include "duel_grid.h"

typedef struct {
    u16 x;
    u16 y;
    u16 field_04;
    u8 pad_06[0xC];
    s16 field_12;
    s32 field_14;
    u8 pad_18[2];
    s16 field_1A;
} DuelEffectObject;

extern u16 D_8009B1D0;
extern s16 D_8009B1D2;
extern u8 D_8009B1D5;
extern s16 D_8009B20C[2];
extern u16 D_8009B220;
extern u8 D_8009B260[8] __attribute__((section(".data")));
extern u8 D_800907D8[];

extern s32 func_80024E24(void);
extern DuelEffectObject *func_8002C604(s32 arg0);
extern void SD_SEPlayFull(s32 arg0);

/* One step of a field-wide stat-penalty sweep, driven once per 16 frames by
 * the D_8009B20C[1] countdown. D_8009B1D0 is the slot index within the acting
 * side's second grid row (slots 5..9); each step spawns a type-0xD effect
 * object at the card's model position, staggers its depth by the slot, and
 * subtracts from the card's stat modifier - 500 when D_8009B1D2 is card 0x15D,
 * 1000 otherwise, with the object state and the drop amount matching. After
 * the fifth slot the sweep sets bit 0x40 of D_8009B220 and then waits there
 * until bit 0 of D_8009B260 clears. */
void func_80025D30(void) {
    DuelCardRecord *record;
    DuelEffectObject *object;
    u8 *card;
    s32 timer;
    s32 base_slot;

    if (func_80024E24() == 0) {
        D_8009B20C[1] = 0;
        D_8009B1D0 = 0;
    }

    if ((D_8009B220 & 0x40) != 0) {
        if ((D_8009B260[0] & 1) == 0) {
            D_8009B220 = 0;
        }
        return;
    }

    timer = *(u16 *)&D_8009B20C[1] - 1;
    D_8009B20C[1] = timer;
    if ((s16)timer > 0) {
        return;
    }
    D_8009B20C[1] = 0x10;

    base_slot = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                DUEL_FIELD_ROW_SIZE;
    record = &D_801A7AD8[D_800907D8[(s16)D_8009B1D0 + base_slot]];
    if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
        card = (u8 *)record->object;
        object = func_8002C604(0xD);
        object->x = *(u16 *)(card + 0x30);
        object->y = *(u16 *)(card + 0x32);
        object->field_04 = *(u16 *)(card + 0x34);
        object->field_14 = object->field_14 + ((s16)D_8009B1D0 << 14);
        if (D_8009B1D2 == 0x15D) {
            object->field_1A = 2;
            record->stat_modifier = record->stat_modifier - 0x1F4;
            object->field_12 = -0x1F4;
        } else {
            object->field_1A = 1;
            record->stat_modifier = record->stat_modifier - 0x3E8;
            object->field_12 = -0x3E8;
        }
        SD_SEPlayFull(0x21);
    }

    D_8009B1D0 = D_8009B1D0 + 1;
    if ((s16)D_8009B1D0 >= DUEL_FIELD_ROW_SIZE) {
        D_8009B220 = D_8009B220 | 0x40;
    }
}
