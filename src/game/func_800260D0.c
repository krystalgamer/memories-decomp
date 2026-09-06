#include "../types.h"
#include "duel_card.h"
#include "duel_grid.h"

typedef struct {
    u16 x;
    u16 y;
    u16 field_04;
    u8 pad_06[0xE];
    s32 field_14;
    u8 pad_18[2];
    s16 field_1A;
} DuelEffectObject;

extern u16 D_8009B1D0;
extern u8 D_8009B1D5;
extern s16 D_8009B20C[2];
extern u16 D_8009B220;
extern u8 D_800907D8[];
extern DuelFieldPosition D_80090800[];

extern s32 func_80024E24(void);
extern DuelEffectObject *func_8002C604(s32 arg0);
extern void SD_SEPlayFull(s32 arg0);

/* One step of a field-wide effect sweep, driven once per 16 frames by the
 * D_8009B1D0 countdown. The first entry resets the step index in
 * D_8009B20C[1]; every later entry advances it and spawns a type-8 effect
 * object over the next slot of the third grid row (slots 10..14 of the acting
 * side), offsetting the object's depth by the step so the objects stagger.
 * Steps past the fifth clear D_8009B220 and end the sweep. A card sitting in
 * the swept slot with a negative stat modifier has it cleared and gets the
 * alternate object state 5. */
void func_800260D0(void) {
    DuelCardRecord *record;
    DuelEffectObject *object;
    u8 *position;
    u8 *positions;
    s32 card;
    s32 next;
    s32 base_slot;
    s32 x;
    s32 timer;
    s32 step;
    register u8 *grid __asm__("$4");

    if (func_80024E24() == 0) {
        D_8009B20C[1] = -1;
        D_8009B1D0 = 0;
    }

    timer = D_8009B1D0 - 1;
    D_8009B1D0 = timer;
    if ((s16)timer > 0) {
        return;
    }

    D_8009B1D0 = 0x10;
    next = *(u16 *)&D_8009B20C[1] + 1;
    D_8009B20C[1] = next;
    if ((s16)next < DUEL_FIELD_ROW_SIZE) {
        grid = D_800907D8;
        base_slot = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                    DUEL_FIELD_SIDE_ZONE_COUNT;
        card = grid[(s16)next + base_slot];
        record = &D_801A7AD8[card];
        object = func_8002C604(8);
        positions = (u8 *)D_80090800;
        step = D_8009B20C[1];
        object->field_1A = 3;
        position = (
            (step + DUEL_FIELD_SIDE_ZONE_COUNT) * sizeof(DuelFieldPosition) +
            D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES
        ) + positions;
        x = *(u16 *)(position + 0);
        object->y = 0;
        object->x = x;
        object->field_04 = *(u16 *)(position + 2);
        object->field_14 = object->field_14 + step * 0x3000;
        SD_SEPlayFull(0x14);

        if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            if (record->stat_modifier < 0) {
                record->stat_modifier = 0;
                object->field_1A = 5;
            }
        }
    } else {
        D_8009B220 = 0;
    }
}
