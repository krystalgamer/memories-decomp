#include "../types.h"
#include "duel_side_state.h"
#include "ai.h"
#include "duel_card.h"
#include "duel_selection_layout.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"

typedef struct {
    u8 pad_00[0x16];
    u8 field_16;
    u8 pad_17[0x19];
    s16 field_30;
    s16 field_32;
} FieldObject;

typedef struct {
    FieldObject *object;
    u8 pad_04[0xB];
    s8 x;
    s8 y;
    u8 pad_11[3];
    u8 field_14;
    u8 pad_15[1];
    s8 field_16;
    u8 field_17;
    u8 field_18;
} FieldSource;

typedef struct {
    u8 pad_00[0x59];
    u8 field_59;
} TextBox;

extern u8 D_8009B1D5;
extern s8 *D_8009B1C8;
extern u8 D_8009B320 __attribute__((section(".data")));
extern u8 D_8009B344 __attribute__((section(".data")));
extern u8 D_8009B34E __attribute__((section(".data")));
extern u8 D_8009B355 __attribute__((section(".data")));
extern s16 gDuel_wSelectedCardID __attribute__((section(".data")));
extern s32 gDuel_adwCardStats[];
extern u8 D_800E9F48[];
extern s32 D_801D5608[];

extern s32 func_80023090(FieldSource *, u8 *);

void func_80023144(FieldSource *source, s32 index)
{
    DuelCardRecord *record = &D_801A7AD8[index];
    TextBox *box;
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
        if (((stats >> 26) & 0x1F) < 0x14) {
            if (record->flags & 0x200) {
                D_8009B344 = (stats >> 18) & 0xF;
            } else {
                D_8009B344 = (stats >> 22) & 0xF;
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
            if (value == D_8009B1D5 && D_8009B1C8[0x1F] == 0) {
                D_8009B34E = two;
            }
        } else {
            if (record->flags & 0x2000) {
                D_8009B34E = 0;
            }
        }
    }
    if (source->field_17 == 3) {
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
    if (source->field_17 == 2 && source->field_18 != 0) {
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
        source->field_14, style, source->object->field_30 + 0x10,
        source->object->field_32 + source->field_16, 0x120, 0x40
    );
    box->field_59 = source->object->field_16 + 1;
    func_80039A14(box);
}
