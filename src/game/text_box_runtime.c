#define GINPUT_PAD1_PRESSED_IN_DATA
#define D_8009B0C1_IN_DATA
#include "../types.h"
#include "graphics_frame.h"
#include "input.h"
#include "display_effect_process_menu_records.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_process_entries.h"
#include "sound.h"
#include "display_object.h"
#include "display_object_core.h"
#include "text_box_update_channels.h"
#include "text_box_runtime.h"
#include "text_box_blocking_build.h"
#include "duel_effect_update_object_layout.h"
#include "dialog_choice_state.h"
#include "text_constants.h"
#include "text_glyph_lookup.h"

/* Regional value: the choice.flags bit that opens and closes the choice (0x8
 * -> 0x10). The European build (src/game/european/) defines its own. */
#ifndef FUNC_80039794_CHOICE_FLAG
#define FUNC_80039794_CHOICE_FLAG 8
#endif

#ifndef DISPLAY_EFFECT_MENU_CONFIRM_MASK
#define DISPLAY_EFFECT_MENU_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

#ifndef DISPLAY_EFFECT_MENU_CHANNEL_STRIDE
#define DISPLAY_EFFECT_MENU_CHANNEL_STRIDE sizeof(DuelEffectChannel)
#endif

/* The retail body walks two pointers over the same four records: the record
 * base it hands to the per-record calls, and a second cursor parked on the
 * record's 0x30 pair, so the choice object and its flags are reached at
 * displacements 0 and 4. Folding the cursor into the base costs a register and
 * four instructions, so the view stays. */
void TextBox_UpdateChannels(void)
{
    DuelEffectChannel *p;
    ChoiceChannelCursor *q;
    s32 reset_value;
    TextGlyphLookupEntry *table;
    s32 n;
    s32 cnt;
    s32 idx;
    s32 kind;
    s32 arg;
    u16 f;

    p = gText_aChannels;
    n = 4;
    reset_value = -1;
    table = gText_aGlyphLookup;
    q = (ChoiceChannelCursor *)&p->field_30;
    do {
        if (q->choice.flags & 0x8000) {
            gText_PumpedGlyphIndex = 0;
            if ((q->choice.flags & 0x2000) == 0) {
                gText_PumpedGlyphIndex = reset_value;
                cnt = -1;
                for (;;) {
                    TextBox_BuildStep(p);
                    cnt++;
                    f = q->choice.flags;
                    if (f & 0x2000) {
                        if (f & FUNC_80039794_CHOICE_FLAG) {
                            q->choice.obj = Dialog_OpenChoice(p);
                        }
                        break;
                    }
                    if (f & 0x1C00) {
                        goto reset;
                    }
                    if (cnt >= gGraphics_bExtraFrameStep) {
                        break;
                    }
                    continue;
reset:
                    gText_PumpedGlyphIndex = reset_value;
                }
            } else {
                if (q->choice.flags & FUNC_80039794_CHOICE_FLAG) {
                    if (gInput_wPad1Pressed &
                        DISPLAY_EFFECT_MENU_CONFIRM_MASK) {
                        q->choice.flags &= 0xFFFF ^ FUNC_80039794_CHOICE_FLAG;
                        DisplayObject_ReleaseIfPresent(q->choice.obj);
                        q->choice.obj = 0;
                        SD_SEPlayFull(0xB);
                    }
                }
            }
            arg = -1;
            idx = gText_PumpedGlyphIndex;
            if (idx >= 0) {
                kind = table[idx].halves.upper & 7;
                if (kind == 4) {
                    arg = gText_PreviousGlyphKind;
                } else {
                    gText_PreviousGlyphKind = kind;
                    arg = kind;
                }
            }
            DisplayEffect_ProcessMenuRecords(arg);
            DuelEffect_ProcessEntries(p);
        }
        q = (ChoiceChannelCursor *)(
            (u8 *)q + DISPLAY_EFFECT_MENU_CHANNEL_STRIDE);
        n--;
        p = (DuelEffectChannel *)(
            (u8 *)p + DISPLAY_EFFECT_MENU_CHANNEL_STRIDE);
    } while (n != 0);
}

/* These three reach the record through the parameter directly. What used to
 * stand at each use was a cast of the pointer to its own type, left from when
 * the parameter was a byte pointer; removing them is byte-identical, and
 * measured.
 *
 * What does change the code is a typed local. Hoisting the record into one
 * changes the prologue register saves under GCC 2.8.1, which is the
 * constraint the old note here was about. */
void TextBox_SetPos(DuelEffectChannel *record, s32 x, s32 y)
{
    DisplayObject *object;

    object = record->field_28;
    record->field_3C = x;
    record->field_40 = y;
    if (object != 0) {
        object->field_30.h.field_30 = x;
        object->field_30.h.field_32 = y;
    }
    object = record->field_2C;
    if (object != 0) {
        if (object->field_1E == 4)
            TextBox_UpdateOutline(record);
        else {
            object->field_30.h.field_30 = x;
            object->field_30.h.field_32 = y;
        }
    }
    object = record->field_30;
    if (object != 0) {
        if (object->field_1E == 4)
            DuelEffect_UpdateObjectLayout(record);
        else {
            object->field_30.h.field_30 = record->field_3E + x - 16;
            object->field_30.h.field_32 = record->field_42 + y - 16;
        }
    }
}

void TextBox_BuildBlocking(DuelEffectChannel *object)
{
    object->flags_34 |= TEXT_BOX_FLAG_BUILD_REQUESTED;
    do {
        TextBox_BuildStep(object);
    } while (!(object->flags_34 & TEXT_BOX_FLAG_DONE));
}

void TextBox_BuildBlockingWithGlyphScratch(DuelEffectChannel *object)
{
    object->flags_34 |= 0xA00;
    do {
        TextBox_BuildStep(object);
    } while (!(object->flags_34 & TEXT_BOX_FLAG_DONE));
}
