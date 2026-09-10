#define D_8009B34E_IN_DATA
#define D_8009B355_IN_DATA
#define D_8009B320_IN_DATA
#include "../types.h"
#include "card_constants.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "ai.h"
#include "display_object_api.h"
#include "display_projection.h"

#include "duel_card.h"
#include "display_object.h"
#include "display_object_config.h"
#include "duel_effect.h"
#include "duel_selection_layout.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"

/* One field-grid source record, as the three functions here actually reach
   it. The two sources this unit was split across described it twice and
   disagreed in three places; all three are settled by what the matched code
   loads and stores, not by preference.

   Two distinct DisplayObject pointers, not one. func_800234E4 creates an
   object and stores it at +0x04; func_80023144 loads a pointer from +0x00 to
   place its text box. Both accesses are in matched code, so the record
   carries both. Whether the two ever hold the same object is not established
   here, so +0x00 keeps an offset name.

   +0x17 is one field with two former names, `table_index` and `field_17`:
   func_800234E4 adds it to the side offset to pick a graphic, func_80023144
   compares it against 2 and 3 to pick a text-box style. Both readings are a
   small region discriminator, so it keeps the name that says what it selects.

   The third disagreement was func_80023144's own signature - declared taking
   u8 here, defined taking s32 there. The definition wins; the call site
   passes a u8 that promotes. */
typedef struct {
    DisplayObject *field_00;  /* 0x00, read by func_80023144 */
    DisplayObject *object;    /* 0x04, created and stored by func_800234E4 */
    u8 pad_08[7];
    s8 x;                     /* 0x0F */
    s8 y;                     /* 0x10 */
    u8 pad_11[3];
    u8 field_14;              /* 0x14, the text box's slot */
    u8 pad_15[1];
    s8 field_16;              /* 0x16, y bias of the text box */
    u8 table_index;           /* 0x17 */
    u8 field_18;              /* 0x18 */
} DuelFieldDisplaySource;

extern u8 D_8009B344 __attribute__((section(".data")));
extern s16 gDuel_wSelectedCardID __attribute__((section(".data")));
extern s32 D_801D5608[];

extern s32 func_80023090(DuelFieldDisplaySource *, u8 *);

void func_80023144(DuelFieldDisplaySource *source, s32 index)
{
    DuelCardRecord *record = &D_801A7AD8[index];
    DuelEffectChannel *box;
    s32 *table;
    s32 style;
    s32 id;
    s32 value;
    s32 stats;

    D_8009B34E = 0;
    D_8009B355 = 0;
    style = 0x50;
    if (record->flags & 0x8000) {
        table = gDuel_adwCardStats;
        id = (s16)record->card_id;
        stats = *(s32 *)((s32)table + ((id - 1) << 2));
        D_8009B34E = 1;
        gDuel_wSelectedCardID = id;
        if (((stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) <
            CARD_TYPE_MAGIC) {
            if (record->flags & 0x200) {
                D_8009B344 =
                    (stats >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            } else {
                D_8009B344 =
                    (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            }
            D_8009B344 = D_8009B344 + 0x17;
            value = Duel_CalcCardStats(record);
            D_801D5608[0] = (s16)value;
            D_801D5608[1] = value >> 16;
        } else {
            style = 0x51;
        }
        if (index % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE) {
            register s32 two asm("$4") = 2;

            D_8009B34E = two;
            if (record->flags & 0x1000) {
                D_8009B34E = 3;
            }
            value = index >= DUEL_CARD_SIDE_RECORD_COUNT;
            if (value == D_8009B1D5 && D_8009B1C8->field_1F == 0) {
                D_8009B34E = two;
            }
        } else {
            if (record->flags & 0x2000) {
                D_8009B34E = 0;
            }
        }
    }
    if (source->table_index == 3) {
        s32 rank;
        s32 *dst = D_801D5608;

        style += 4;
        rank = D_800E9FF0[D_8009B1D5 ^ 1].field_19;
        dst[2] = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst[2] = 3;
            }
            D_8009B355 = 1;
        }
        if (D_8009B34E != 0) {
            D_8009B355 = D_8009B355 | 2;
            D_8009B320 = func_80023090(
                source, &D_800E9F48[D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE]
            );
        }
    }
    if (source->table_index == 2 && source->field_18 != 0) {
        s32 rank;
        register s32 *dst asm("$4");
        s32 side;

        side = D_8009B1D5;
        if (source->y < 2) {
            side = side ^ 1;
        }
        dst = D_801D5608;
        rank = D_800E9FF0[side].field_19;
        dst[2] = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst[2] = 3;
            }
            D_8009B355 = 1;
        }
        style += 2;
    }
    box = TextBox_Create(
        source->field_14, style,
        *(s16 *)&source->field_00->field_30.h.field_30 + 0x10,
        *(s16 *)&source->field_00->field_30.h.field_32 + source->field_16,
        0x120, 0x40
    );
    box->field_59 = *(u8 *)&source->field_00->field_16 + 1;
    func_80039A14(box);
}

void func_8002348C(DuelFieldDisplaySource *source)
{
    u8 *table = D_800907D8;
    s32 index =
        source->y * DUEL_FIELD_ROW_SIZE + source->x +
        D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;

    func_80023144(source, table[index]);
}

void func_800234E4(DuelFieldDisplaySource *source)
{
    s32 index;
    DisplayObject *object;
    DuelFieldPosition *position;

    index = source->y * DUEL_FIELD_ROW_SIZE + source->x;
    object = func_800400AC(func_8004002C(), 2);
    func_80040468(
        (u8 *)object,
        4,
        3,
        source->table_index + D_8009B1D5 * 4,
        0x1F,
        0x100
    );

    {
        u8 *base = (u8 *)D_80090800;
        s32 offset = index * sizeof(DuelFieldPosition) +
            D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES;

        position = (DuelFieldPosition *)(base + offset);
    }
    object->position.h.field_28 = position->x;
    object->position.h.field_2A = position->y;
    object->flags = object->flags | 0x28;
    object->update = (DisplayObjectCallback)func_80015D18;
    source->object = object;
}
