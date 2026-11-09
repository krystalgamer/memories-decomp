#include "../types.h"
#include "graphics_frame.h"
#include "input.h"
#include "display_effect_process_menu_records.h"
#include "duel_effect.h"

/* Zero-initialized storage this unit owns (0x800EB0F8-0x800EB288 in North
 * America, 0x800EAFD8-0x800EB158 in Japan), defined in address order ahead of
 * the headers that declare it (#2602). */
DuelEffectChannel gText_aChannels[DUEL_EFFECT_CHANNEL_COUNT];
#ifdef TEXT_BOX_RUNTIME_OWNS_BATTLE_MODELS
#include "main_modes.h"
AnimatedBattleModelProperties gMain_aAnimatedBattleModels[2];
#endif

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

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gText_PreviousGlyphKind;
s16 gText_PumpedGlyphIndex;

/* Regional value: the choice.flags bit that opens and closes the choice (0x8
 * -> 0x10). The European build (src/game/european/) defines its own. */
#ifndef FUNC_80039794_CHOICE_FLAG
#define FUNC_80039794_CHOICE_FLAG 8
#endif

#ifndef DISPLAY_EFFECT_MENU_CONFIRM_MASK
#define DISPLAY_EFFECT_MENU_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

/* One record pointer walks the four channels beside a down-counter. Under
 * strength reduction GCC folds the record's prompt_object and flags_34
 * accesses into a second induction register parked at offset 0x30, which is
 * the retail shape. */
void TextBox_UpdateChannels(void)
{
    DuelEffectChannel *p;
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
    do {
        if (p->flags_34 & 0x8000) {
            gText_PumpedGlyphIndex = 0;
            if ((p->flags_34 & 0x2000) == 0) {
                gText_PumpedGlyphIndex = reset_value;
                cnt = -1;
                for (;;) {
                    TextBox_BuildStep(p);
                    cnt++;
                    f = p->flags_34;
                    if (f & 0x2000) {
                        if (f & FUNC_80039794_CHOICE_FLAG) {
                            p->prompt_object = Dialog_OpenChoice(p);
                        }
                        break;
                    }
                    if ((f & 0x1C00) == 0) {
                        if (cnt >= gGraphics_bExtraFrameStep) {
                            break;
                        }
                    } else {
                        gText_PumpedGlyphIndex = reset_value;
                    }
                }
            } else {
                if (p->flags_34 & FUNC_80039794_CHOICE_FLAG) {
                    if (gInput_awPadPressed[0] &
                        DISPLAY_EFFECT_MENU_CONFIRM_MASK) {
                        p->flags_34 &= 0xFFFF ^ FUNC_80039794_CHOICE_FLAG;
                        DisplayObject_ReleaseIfPresent(p->prompt_object);
                        p->prompt_object = 0;
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
        p++;
        n--;
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

    object = record->render_object;
    record->rect_x = x;
    record->rect_y = y;
    if (object != 0) {
        object->xy.h.x = x;
        object->xy.h.y = y;
    }
    object = record->aux_object;
    if (object != 0) {
        if (object->list_key == 4)
            TextBox_UpdateOutline(record);
        else {
            object->xy.h.x = x;
            object->xy.h.y = y;
        }
    }
    object = record->prompt_object;
    if (object != 0) {
        if (object->list_key == 4)
            DuelEffect_UpdateObjectLayout(record);
        else {
            object->xy.h.x = record->rect_width + x - 16;
            object->xy.h.y = record->rect_height + y - 16;
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
