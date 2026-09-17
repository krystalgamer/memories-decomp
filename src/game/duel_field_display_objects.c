#define D_8009B34E_IN_DATA
#define D_8009B355_IN_DATA
#define D_8009B320_IN_DATA
#define D_8009B344_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "card_constants.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "ai.h"
#include "display_object_core.h"
#include "display_projection.h"

#include "duel_card.h"
#include "display_object.h"
#include "display_object_config.h"
#include "duel_field_display_objects.h"
#include "duel_effect.h"
#include "duel_selection_layout.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"
#include "display_object_helpers.h"
#include "display_object_interpolation.h"
#include "duel_card_object_helpers.h"
#include "duel_card_staging.h"
#include "duel_card_turn_animations.h"
#include "duel_side_view_angles.h"
#include "func_80022D94.h"
#include "view_state.h"
#include "../unmatched.h"

s32 func_80023090(DuelFieldCursor *cursor_a, DuelFieldCursor *cursor_b)
{
    u8 *grid = D_800907D8;
    DuelCardRecord *records;
    s32 page;
    s32 offset_a;
    s32 row_b;
    s32 index_a;
    s32 index_b;
    s32 slot_a;
    s32 slot_b;
    s32 side;
    s32 order;

    row_b = cursor_b->row;
    index_a = cursor_a->row * DUEL_FIELD_ROW_SIZE + cursor_a->col;
    side = D_8009B1D5;
    page = side * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    slot_a = grid[index_a + page];
    offset_a = slot_a * sizeof(DuelCardRecord);
    records = D_801A7AD8;
    index_b = row_b * DUEL_FIELD_ROW_SIZE + cursor_b->col;
    slot_b = grid[index_b + page];

    order = Duel_CalcGuardianStarBonus(
        (DuelCardRecord *)((u8 *)records + offset_a),
        &records[slot_b]
    );

    if (order == 0) {
        return 4;
    }
    if (order < 0) {
        return 1;
    }
    return 6;
}

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
            D_801D5608[0].card_stats.attack = (s16)value;
            D_801D5608[0].card_stats.defense = value >> 16;
        } else {
            style = 0x51;
        }
        if (index % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE) {
            D_8009B34E = 2;
            if (record->flags & 0x1000) {
                D_8009B34E = 3;
            }
            value = index >= DUEL_CARD_SIDE_RECORD_COUNT;
            if (value == D_8009B1D5 &&
                D_8009B1C8->card_view_mode == 0) {
                D_8009B34E = 2;
            }
        } else {
            if (record->flags & 0x2000) {
                D_8009B34E = 0;
            }
        }
    }
    value = source->table_index;
    if (value == 3) {
        s32 rank;
        TextStagingValues *dst = D_801D5608;

        style += 4;
        rank = D_800E9FF0[D_8009B1D5 ^ 1].swords_turns_remaining;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
            }
            D_8009B355 = 1;
        }
        if (D_8009B34E != 0) {
            D_8009B355 = D_8009B355 | 2;
            D_8009B320 = func_80023090(
                (DuelFieldCursor *)source,
                (DuelFieldCursor *)
                    &D_800E9F48[D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE]
            );
        }
    }
    if (source->table_index == 2 && source->field_18 != 0) {
        s32 rank;
        TextStagingValues *dst;
        s32 side;

        side = D_8009B1D5;
        if (source->y < 2) {
            side = side ^ 1;
        }
        dst = D_801D5608;
        rank = D_800E9FF0[side].swords_turns_remaining;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
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
    func_80039A14((struct DuelEffectChannel *)box);
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
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteResource(
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
    object->flags = object->flags |
                    DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    object->update = (DisplayObjectCallback)func_80015D18;
    source->object = object;
}

/* The empty do-while loops are scheduling barriers. The first keeps the
 * D_8009B162 load above the entry test and `result = 0` below the view
 * address, so the zero lands in $v0 for the early exit. The second follows
 * the 0x8000 test, so `busy = 0` stays a literal zero rather than a copy of
 * that $v0. Each replay loop has its own D_8015C424 pointer, set once and
 * read once, as retail's loop-entry address loads require. */
s32 func_800235C0(void)
{
    DuelFieldDisplaySource *source;
    ViewState *view;
    DisplayObject *o;
    s32 i;
    s32 moving;
    s32 busy;
    s32 x;
    s32 y;
    s32 z;
    s32 w;
    s32 t;
    s16 count;
    u8 *records;
    u8 *replay;
    s32 result;
    u16 flags;

    flags = D_8009B162;
    source = (DuelFieldDisplaySource *)D_8009B1B4;
    view = &D_800F2848;
    do {
    } while (0);
    result = 0;
    if (flags != 0) {
        x = flags & 0x8000;
        do {
        } while (0);
        busy = 0;
        if (x == 0) {
            D_8009B162 = flags | 0x8000;
            if (D_8009B162 & 1) {
                func_80022D94(0x10, 0x14E, 0x3FE, D_8009AF20[D_8009B1D5],
                              D_800907AC[D_8009B1D5][source->field_18][source->y]);
            }
            if (D_8009B162 & 4) {
                func_80022D94(0x10, 0x258, 0x100, D_8009AF20[D_8009B1D5], 0);
                DisplayObject_ReleaseIfPresent(source->object);
                source->object = 0;
                records = D_8015C424;
                for (i = 5; i < 30; i++) {
                    if (i % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE &&
                        (D_801A7AD8[i].flags & 0x8000)) {
                        o = ((DuelCardReplayRecordBlock *)(records +
                            i * sizeof(DuelCardRecord) +
                            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET))->record.object;
                        o->field_6C = 1;
                        o->field_60 = 4;
                        o->update = (DisplayObjectCallback)func_800229F4;
                    }
                }
                D_8009B162 |= 0x4000;
            }
            if (D_8009B162 & 2) {
                o = func_80018150(0, 0xF0);
                source->field_16 = 0x24;
                if (source->field_13 != 0) {
                    DisplayObject_SetResourceVariant((DisplayObjectConfig *)o, source->field_13);
                    source->field_16 = 7;
                }
                DisplayObject_SetDepthOffset((u8 *)o, -(source->table_index * 2) - 2);
                o->position.h.field_28 = 0;
                o->position.h.field_2A = source->field_0C;
                DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
                o->field_60 = -0x400;
                o->field_6C = 2;
                source->field_00 = o;
                if (source->entries != 0) {
                    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                        if (source->entries[i].object != 0) {
                            source->entries[i].object->field_30.h.field_30 =
                                o->field_30.h.field_30 + (i * 0x3C + 0xE);
                            source->entries[i].object->field_30.h.field_32 =
                                o->field_30.h.field_32 - 0x1C;
                        }
                    }
                }
                DuelSelection_LinkDisplayObjects((DisplayParent *)source, 0);
            }
            if (D_8009B162 & 8) {
                o = source->field_00;
                o->position.h.field_2A = 0xF0;
                o->position.h.field_28 = o->field_30.h.field_30;
                DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
                o->field_60 = 0x400;
                o->field_6C = 2;
                DuelSelection_LinkDisplayObjects((DisplayParent *)source, 1);
            }
            busy = 0;
        }
        moving = busy;
        if (D_8009B162 & 0x40) {
            o = source->object;
            if (o->field_60 != 0) {
                t = (*(s16 *)&o->position.h.field_28 << 8) | o->pad_62[0];
                t += o->field_34.h.field_36;
                o->pad_62[0] = t;
                o->position.h.field_28 = t >> 8;
                t = (*(s16 *)&o->position.h.field_2A << 8) | o->field_64;
                t += o->field_38.h.field_3A;
                o->position.h.field_2A = t >> 8;
                o->field_64 = t;
                o->field_60--;
                busy = 1;
                if (o->field_60 <= 0) {
                    busy = 0;
                    o->field_60 = 0;
                    o->position.word = o->field_2C.word;
                }
            }
        }
        o = source->field_00;
        if ((D_8009B162 & 2) && o->field_6C != 0) {
            busy = 1;
            moving = busy;
            Widget_SlideSine((DisplayObjectPosition *)o,
                             *(s16 *)&o->position.h.field_28,
                             *(s16 *)&o->position.h.field_2A, o->field_60);
            o->field_60 += 0x40;
            if (o->field_60 >= 0) {
                o->field_6C = 0;
                o->field_30.word = o->position.word;
            }
        }
        if ((D_8009B162 & 8) && o->field_6C != 0) {
            busy = 1;
            Widget_SlideSine((DisplayObjectPosition *)o,
                             *(s16 *)&o->position.h.field_28,
                             *(s16 *)&o->position.h.field_2A, o->field_60);
            o->field_60 -= 0x40;
            moving = busy;
            if (o->field_60 < 0) {
                TextBox_Destroy(&D_800EB0F8[source->field_14]);
                DisplayObject_ReleaseIfPresent(o);
                source->field_00 = 0;
                if (source->entries != 0) {
                    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                        DisplayObject_ReleaseIfPresent(source->entries[i].object);
                        source->entries[i].object = 0;
                        DisplayObject_ReleaseIfPresent(source->entries[i].field_04);
                        source->entries[i].field_04 = 0;
                    }
                }
                o->field_6C = 0;
                moving = 0;
            }
        }
        if (moving != 0) {
            TextBox_SetPos(&D_800EB0F8[source->field_14],
                           *(s16 *)&o->field_30.h.field_30 + 0x10,
                           *(s16 *)&o->field_30.h.field_32 + source->field_16);
        }
        if (D_8009B162 & 0x4000) {
            busy = 1;
            if (func_80042B40(1) != 0) {
                return busy;
            }
            D_8009B162 &= 0xBFFF;
        }
        if (D_8009B204 != 0) {
            count = (u16)D_8009B204 - 1;
            busy = 1;
            D_8009B204 = count;
            x = D_8009B1C4 += D_8009B1FC;
            y = D_8009B15C += D_8009B198;
            z = D_8009B158 += D_8009B194;
            w = D_8009B224 += D_8009B168;
            view->field_00 = x >> 16;
            view->field_04 = y >> 16;
            view->angle = z >> 16;
            view->view.vrz = w >> 16;
            if (count == 0) {
                view->field_00 = D_8009B1EE;
                view->field_04 = D_8009B192;
                view->angle = D_8009B190;
                view->view.vrz = D_8009B166;
                if (D_8009B162 & 1) {
                    replay = D_8015C424;
                    for (i = 5; i < 30; i++) {
                        if (i % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE &&
                            (D_801A7AD8[i].flags & 0x8000)) {
                            o = ((DuelCardReplayRecordBlock *)(replay +
                                i * sizeof(DuelCardRecord) +
                                DUEL_CARD_STAGING_REPLAY_BASE_OFFSET))->record.object;
                            o->field_6C = 1;
                            o->field_60 = 4;
                            o->update = (DisplayObjectCallback)func_80022674;
                        }
                    }
                    D_8009B162 |= 0x4000;
                }
            }
            func_8001352C();
        }
        if (busy == 0) {
            if (D_8009B162 & 1) {
                func_800234E4(source);
                func_8002348C(source);
            }
            D_8009B162 = 0;
        }
        result = busy;
    }
    return result;
}
