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
#include "display_object_core.h"
#include "text_box_update_channels.h"
#include "text_box_runtime.h"
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
