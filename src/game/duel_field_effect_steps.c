/* Two steps of the same field-wide duel effect machinery. Both latch their
 * first frame through DuelEffect_MarkInitialized, both drive their phase
 * through D_8009B220, and both walk the acting side's grid D_800907D8 by
 * D_8009B1D5 into the card records at D_801A7AD8. */
#define D_8009B1D5_IS_VOLATILE
#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "../unmatched.h"
#include "duel_grid.h"
#include "func_8002C604.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "duel_side_state.h"
#include "duel_card.h"
#include "duel_effect.h"
#include "display_object_api.h"
#include "sound.h"
#include "duel_field_effect_steps.h"

typedef struct Object {
    u16 x, y;
    u8 pad_04[0x16];
    u16 field_1A;
    u8 flags, count;
    u8 pad_1E[6];
    void (*callback)(struct Object *);
    u8 pad_28[0x44];
    u8 active;
} Object;
extern Object *D_8009B1F0[2];
extern void func_80025B28(Object *);

void func_80025F3C(void)
{
    Object *object;
    DuelCardRecord *entry;
    int slot;
    register int side __asm__("$3");
    if (DuelEffect_MarkInitialized() == 0) {
        object = (Object *)func_8002C604(0x15);
        side = D_8009B1D5 ^ 1;
        D_8009B17C = (u8 *)object;
        D_8009B1F0[side] = object;
        object->flags |= 0x20;
        object->x = 0xA0;
        object->y = 0x78;
        side = D_8009B1D5 ^ 1;
        object->field_1A = side;
        SD_SEPlayFull(0x23);
    } else if (!(D_8009B220 & 0x40)) {
        if (((Object *)D_8009B17C)->count != 0) {
            D_8009B220 |= 0x40;
            for (
                slot = DUEL_FIELD_ROW_SIZE;
                slot < DUEL_FIELD_SIDE_ZONE_COUNT;
                slot++
            ) {
                entry = &D_801A7AD8[D_800907D8[D_8009B1D5][slot]];
                if ((*(u32 *)&entry->terrain_modifier & 0x90000000) == 0x90000000) {
                    register Object *current __asm__("$2");
                    current = entry->object;
                    current->callback = func_80025B28;
                    current->active = 1;
                }
            }
        }
    } else if (func_80042B40(1) == 0 && ((Object *)D_8009B17C)->count >= 2) {
        D_800E9FF0[D_8009B1D5 ^ 1].field_19 = 4;
        D_8009B220 = 0;
    }
}

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

    if (DuelEffect_MarkInitialized() == 0) {
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
        /* This step walks the grid flat, with the side folded into base_slot
           below, while func_80025F3C above uses the two-dimensional view the
           unit's DUEL_FIELD_GRID_2D declaration gives. The cast is the one
           place the two spellings meet. */
        grid = (u8 *)D_800907D8;
        base_slot = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                    DUEL_FIELD_SIDE_ZONE_COUNT;
        card = grid[(s16)next + base_slot];
        record = &D_801A7AD8[card];
        object = (DuelEffectObject *)func_8002C604(8);
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
