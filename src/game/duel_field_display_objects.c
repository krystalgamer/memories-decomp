#define D_8009B34E_IN_DATA
#define D_8009B355_IN_DATA
#define D_8009B320_IN_DATA
#define D_8009B344_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "text_box_blocking_build.h"
#include "card_constants.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "ai.h"
#include "display_object_core.h"
#include "display_projection.h"

#include "duel_card.h"
#include "card_presentation.h"
#include "display_object.h"
#include "display_object_config.h"
#include "duel_field_display_objects.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_selection_layout.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "display_object_helpers.h"
#include "display_object_interpolation.h"
#include "duel_card_object_helpers.h"
#include "duel_card_staging.h"
#include "duel_card_turn_animations.h"
#include "duel_side_view_angles.h"
#include "duel_view_tween.h"
#include "view_state.h"
#include "../unmatched.h"
#include "view_state_current.h"
#include "sound.h"
#include "duel_cursor_grid_direction.h"

/* Regional value: the 0xF0 passed to Duel_CreateFieldCardPanel and stored in position
 * field_2A, the screen height (0xF0 -> 0x100). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_FIELD_SCREEN_HEIGHT
#define DUEL_FIELD_SCREEN_HEIGHT 0xF0
#endif

s32 DuelField_GetGuardianStarDisplayCode(DuelSelectionRecord *cursor_a, DuelSelectionRecord *cursor_b)
{
    u8 *grid = gDuel_abFieldGridCardRecordIndices;
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
    side = gDuel_bActiveSide;
    page = side * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    slot_a = grid[index_a + page];
    /* Retail computes the typed byte offset before loading the record base. */
    offset_a = slot_a * sizeof(DuelCardRecord);
    records = gDuel_aCardRecords;
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

void DuelField_CreateCardInfoTextBox(DuelSelectionRecord *source, s32 card_record_index)
{
    DuelCardRecord *record = &gDuel_aCardRecords[card_record_index];
    DuelEffectChannel *box;
    s32 *table;
    s32 style;
    s32 id;
    s32 value;
    s32 stats;

    gText_BranchSelectorA = 0;
    gText_BranchSelectorB = 0;
    style = 0x50;
    if (record->flags & 0x8000) {
        table = gDuel_adwCardStats;
        id = (s16)record->card_id;
        stats = *(s32 *)((s32)table + ((id - 1) << 2));
        gText_BranchSelectorA = 1;
        gDuel_wSelectedCardID = id;
        if (((stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) <
            CARD_TYPE_MAGIC) {
            if (record->flags & 0x200) {
                gText_GuardianGlyphCode =
                    (stats >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            } else {
                gText_GuardianGlyphCode =
                    (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            }
            gText_GuardianGlyphCode = gText_GuardianGlyphCode + 0x17;
            value = Duel_CalcCardStats(record);
            gText_StagedValues[0].card_stats.attack = (s16)value;
            gText_StagedValues[0].card_stats.defense = value >> 16;
        } else {
            style = 0x51;
        }
        if (card_record_index % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE) {
            gText_BranchSelectorA = 2;
            if (record->flags & 0x1000) {
                gText_BranchSelectorA = 3;
            }
            value = card_record_index >= DUEL_CARD_SIDE_RECORD_COUNT;
            if (value == gDuel_bActiveSide &&
                gDuel_pActiveSideState->card_view_mode == 0) {
                gText_BranchSelectorA = 2;
            }
        } else {
            if (record->flags & 0x2000) {
                gText_BranchSelectorA = 0;
            }
        }
    }
    value = source->record_index;
    if (value == 3) {
        s32 rank;
        TextStagingValues *dst = gText_StagedValues;

        style += 4;
        rank = gDuel_aSideStates[gDuel_bActiveSide ^ 1].swords_turns_remaining;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
            }
            gText_BranchSelectorB = 1;
        }
        if (gText_BranchSelectorA != 0) {
            gText_BranchSelectorB = gText_BranchSelectorB | 2;
            gText_SelectedCardColorIndex = DuelField_GetGuardianStarDisplayCode(
                source,
                (DuelSelectionRecord *)
                    &gDuel_abSelectionRecord2[gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE]
            );
        }
    }
    if (source->record_index == 2 && source->field_18 != 0) {
        s32 rank;
        TextStagingValues *dst;
        s32 side;

        side = gDuel_bActiveSide;
        if (source->row < 2) {
            side = side ^ 1;
        }
        dst = gText_StagedValues;
        rank = gDuel_aSideStates[side].swords_turns_remaining;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
            }
            gText_BranchSelectorB = 1;
        }
        style += 2;
    }
    box = TextBox_Create(
        source->field_14, style,
        (s16)source->field_00->field_30.h.field_30 + 0x10,
        (s16)source->field_00->field_30.h.field_32 + source->text_y_offset,
        0x120, 0x40
    );
    box->depth_offset = *(u8 *)&source->field_00->depth_offset + 1;
    TextBox_BuildBlocking((struct DuelEffectChannel *)box);
}

void DuelField_RefreshCursorCardInfo(DuelSelectionRecord *source)
{
    u8 *table = gDuel_abFieldGridCardRecordIndices;
    s32 index =
        source->row * DUEL_FIELD_ROW_SIZE + source->col +
        gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;

    DuelField_CreateCardInfoTextBox(source, table[index]);
}

void Duel_CreateFieldCursorObject(DuelSelectionRecord *source)
{
    s32 index;
    DisplayObject *object;
    DuelFieldPosition *position;

    index = source->row * DUEL_FIELD_ROW_SIZE + source->col;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteResource(
        object,
        4,
        3,
        source->record_index + gDuel_bActiveSide * 4,
        0x1F,
        0x100
    );

    {
        u8 *base = (u8 *)gDuel_aFieldGridGroundPositions;
        s32 offset = (u32)&((DuelFieldPosition *)0)[index] +
            gDuel_bActiveSide * DUEL_FIELD_SIDE_POSITION_BYTES;

        position = (DuelFieldPosition *)(base + offset);
    }
    object->position.h.field_28 = position->x;
    object->position.h.field_2A = position->y;
    object->flags = object->flags |
                    DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    object->update = (DisplayObjectCallback)DisplayObject_ProjectGroundPosition;
    source->cursor_object = object;
}

#ifndef DUEL_FIELD_DISPLAY_CHANNEL_TYPE
#define DUEL_FIELD_DISPLAY_CHANNEL_TYPE DuelEffectChannel
#endif
#define DUEL_FIELD_DISPLAY_CHANNEL(index) \
    ((DuelEffectChannel *)&((DUEL_FIELD_DISPLAY_CHANNEL_TYPE *)gText_aChannels)[index])

/* The empty do-while loops are scheduling barriers. The first keeps the
 * gDuel_wDisplayTransitionFlags load above the entry test and `result = 0` below the view
 * address, so the zero lands in $v0 for the early exit. The second follows
 * the 0x8000 test, so `busy = 0` stays a literal zero rather than a copy of
 * that $v0. Each replay loop has its own gDuel_abCardStagingBuffer pointer, set once and
 * read once, as retail's loop-entry address loads require. */
s32 Duel_UpdateDisplayTransition(void)
{
    DuelSelectionRecord *source;
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

    flags = gDuel_wDisplayTransitionFlags;
    source = gDuel_pActiveSelectionRecord;
    view = &gViewState_Current;
    do {
    } while (0);
    result = 0;
    if (flags != 0) {
        x = flags & 0x8000;
        do {
        } while (0);
        busy = 0;
        if (x == 0) {
            gDuel_wDisplayTransitionFlags = flags | 0x8000;
            if (gDuel_wDisplayTransitionFlags & 1) {
                Duel_StartViewTween(0x10, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                              gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][source->field_18][source->row]);
            }
            if (gDuel_wDisplayTransitionFlags & 4) {
                Duel_StartViewTween(0x10, 0x258, 0x100, gDuel_awSideViewHeadings[gDuel_bActiveSide], 0);
                DisplayObject_ReleaseIfPresent(source->cursor_object);
                source->cursor_object = 0;
                records = gDuel_abCardStagingBuffer;
                for (i = 5; i < 30; i++) {
                    if (i % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE &&
                        (gDuel_aCardRecords[i].flags & 0x8000)) {
                        o = ((DuelCardReplayRecordBlock *)(
                            (u8 *)&((DuelCardRecord *)records)[i] +
                            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET))->record.object;
                        o->field_6C = 1;
                        o->field_60 = 4;
                        o->update = (DisplayObjectCallback)Duel_UpdateCardToFieldView;
                    }
                }
                gDuel_wDisplayTransitionFlags |= 0x4000;
            }
            if (gDuel_wDisplayTransitionFlags & 2) {
                o = Duel_CreateFieldCardPanel(0, DUEL_FIELD_SCREEN_HEIGHT);
                source->text_y_offset = 0x24;
                if (source->field_13 != 0) {
                    DisplayObject_SetResourceVariant((DisplayObjectConfig *)o, source->field_13);
                    source->text_y_offset = 7;
                }
                DisplayObject_SetDepthOffset(o, -(source->record_index * 2) - 2);
                o->position.h.field_28 = 0;
                o->position.h.field_2A = source->field_0C;
                DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
                o->field_60 = -0x400;
                o->field_6C = 2;
                source->field_00 = o;
                if (source->hand != 0) {
                    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                        if (source->hand[i].object != 0) {
                            source->hand[i].object->field_30.h.field_30 =
                                o->field_30.h.field_30 + (i * 0x3C + 0xE);
                            source->hand[i].object->field_30.h.field_32 =
                                o->field_30.h.field_32 - 0x1C;
                        }
                    }
                }
                DuelSelection_LinkDisplayObjects(source, 0);
            }
            if (gDuel_wDisplayTransitionFlags & 8) {
                o = source->field_00;
                o->position.h.field_2A = DUEL_FIELD_SCREEN_HEIGHT;
                o->position.h.field_28 = o->field_30.h.field_30;
                DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
                o->field_60 = 0x400;
                o->field_6C = 2;
                DuelSelection_LinkDisplayObjects(source, 1);
            }
            busy = 0;
        }
        moving = busy;
        if (gDuel_wDisplayTransitionFlags & 0x40) {
            o = source->cursor_object;
            if (o->field_60 != 0) {
                t = ((s16)o->position.h.field_28 << 8) | o->pad_62[0];
                t += o->field_34.h.field_36;
                o->pad_62[0] = t;
                o->position.h.field_28 = t >> 8;
                t = ((s16)o->position.h.field_2A << 8) | o->field_64;
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
        if ((gDuel_wDisplayTransitionFlags & 2) && o->field_6C != 0) {
            busy = 1;
            moving = busy;
            Widget_SlideSine((DisplayObjectPosition *)o,
                             (s16)o->position.h.field_28,
                             (s16)o->position.h.field_2A, o->field_60);
            o->field_60 += 0x40;
            if (o->field_60 >= 0) {
                o->field_6C = 0;
                o->field_30.word = o->position.word;
            }
        }
        if ((gDuel_wDisplayTransitionFlags & 8) && o->field_6C != 0) {
            busy = 1;
            Widget_SlideSine((DisplayObjectPosition *)o,
                             (s16)o->position.h.field_28,
                             (s16)o->position.h.field_2A, o->field_60);
            o->field_60 -= 0x40;
            moving = busy;
            if (o->field_60 < 0) {
                TextBox_Destroy(DUEL_FIELD_DISPLAY_CHANNEL(source->field_14));
                DisplayObject_ReleaseIfPresent(o);
                source->field_00 = 0;
                if (source->hand != 0) {
                    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                        DisplayObject_ReleaseIfPresent(source->hand[i].object);
                        source->hand[i].object = 0;
                        DisplayObject_ReleaseIfPresent(source->hand[i].order_marker);
                        source->hand[i].order_marker = 0;
                    }
                }
                o->field_6C = 0;
                moving = 0;
            }
        }
        if (moving != 0) {
            TextBox_SetPos(DUEL_FIELD_DISPLAY_CHANNEL(source->field_14),
                           (s16)o->field_30.h.field_30 + 0x10,
                           (s16)o->field_30.h.field_32 + source->text_y_offset);
        }
        if (gDuel_wDisplayTransitionFlags & 0x4000) {
            busy = 1;
            if (DisplayObject_FindAllocatedByTag(1) != 0) {
                return busy;
            }
            gDuel_wDisplayTransitionFlags &= 0xBFFF;
        }
        if (gDuel_wViewTweenFramesRemaining != 0) {
            count = (u16)gDuel_wViewTweenFramesRemaining - 1;
            busy = 1;
            gDuel_wViewTweenFramesRemaining = count;
            x = gDuel_dwViewTweenDistanceAccumulator += gDuel_dwViewTweenDistanceStep;
            y = gDuel_dwViewTweenOrbitAngleAccumulator += gDuel_dwViewTweenOrbitAngleStep;
            z = gDuel_dwViewTweenHeadingAccumulator += gDuel_dwViewTweenHeadingStep;
            w = gDuel_dwViewTweenReferenceZAccumulator += gDuel_dwViewTweenReferenceZStep;
            view->field_00 = x >> 16;
            view->field_04 = y >> 16;
            view->angle = z >> 16;
            view->view.vrz = w >> 16;
            if (count == 0) {
                view->field_00 = gDuel_wViewTweenDistanceTarget;
                view->field_04 = gDuel_wViewTweenOrbitAngleTarget;
                view->angle = gDuel_wViewTweenHeadingTarget;
                view->view.vrz = gDuel_wViewTweenReferenceZTarget;
                if (gDuel_wDisplayTransitionFlags & 1) {
                    replay = gDuel_abCardStagingBuffer;
                    for (i = 5; i < 30; i++) {
                        if (i % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE &&
                            (gDuel_aCardRecords[i].flags & 0x8000)) {
                            o = ((DuelCardReplayRecordBlock *)(
                                (u8 *)&((DuelCardRecord *)replay)[i] +
                                DUEL_CARD_STAGING_REPLAY_BASE_OFFSET))->record.object;
                            o->field_6C = 1;
                            o->field_60 = 4;
                            o->update = (DisplayObjectCallback)Duel_UpdateCardToSelectionView;
                        }
                    }
                    gDuel_wDisplayTransitionFlags |= 0x4000;
                }
            }
            ViewState_ApplyOrbit();
        }
        if (busy == 0) {
            if (gDuel_wDisplayTransitionFlags & 1) {
                Duel_CreateFieldCursorObject(source);
                DuelField_RefreshCursorCardInfo(source);
            }
            gDuel_wDisplayTransitionFlags = 0;
        }
        result = busy;
    }
    return result;
}

void DuelCursor_ProcessGridDirection(DuelSelectionRecord *o, s32 dir) {
    DisplayObject *d;
    s32 pos;
    s32 shift;
    s32 index;
    s32 q;

    d = o->cursor_object;
    if (o->status & 0x80) {
        if (gDuel_wDisplayTransitionFlags != 0) {
            return;
        }
        /* The record is deliberately left in $a0 rather than passed. */
        ((void (*)(void))DuelField_RefreshCursorCardInfo)();
        o->status &= 0x3F;
        SD_SEPlayFull(6);
        return;
    }
    if (dir < 0) {
        return;
    }
    if (dir & 1) {
        pos = o->row + 1;
        if (dir & 2) {
            pos = o->row - 1;
        }
        if (pos >= (s8)o->row_end) {
            return;
        }
        if (pos < (s8)o->row_min) {
            return;
        }
        shift = 0x10;
        if ((o->row ^ pos) != 3) {
            shift = 8;
        }
        Duel_StartViewTween(shift, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                      gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][o->field_18][pos]);
        o->row = pos;
        index = pos * DUEL_FIELD_ROW_SIZE + o->col;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        d->field_6C = 0;
        d->field_60 = shift;
        d->field_2C.h.field_2C = (s16)d->position.h.field_28;
        d->field_2C.h.field_2E = gDuel_aFieldGridGroundPositions[gDuel_bActiveSide][index].y;
        q = ((d->field_2C.h.field_2E - (s16)d->position.h.field_2A) << 8) / shift;
        gDuel_wDisplayTransitionFlags = 0x50;
        d->field_38.h.field_3A = q;
        o->status |= 0xC0;
    } else {
        pos = o->col + 1;
        if (dir & 2) {
            pos = o->col - 1;
        }
        if ((u32)pos >= DUEL_FIELD_ROW_SIZE) {
            return;
        }
        o->col = pos;
        index = o->row * DUEL_FIELD_ROW_SIZE + (s8)pos;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        shift = 8;
        d->field_60 = shift;
        d->field_6C = 0;
        d->field_2C.h.field_2C = gDuel_aFieldGridGroundPositions[gDuel_bActiveSide][index].x;
        gDuel_wDisplayTransitionFlags = 0x40;
        d->field_2C.h.field_2E = (s16)d->position.h.field_2A;
        d->field_34.h.field_36 =
            (((s16)d->field_2C.h.field_2C - (s16)d->position.h.field_28) << shift) / shift;
        o->status |= 0x80;
    }
}
