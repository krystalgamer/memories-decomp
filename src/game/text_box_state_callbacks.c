#include "../types.h"
#include "text_box_state_callbacks.h"
#include "dialog_choice_state.h"
#include "duel_effect_entry_control.h"
#include "duel_effect_state_callbacks.h"
#include "../unmatched.h"

/* Initialized data at 0x80090E64: the text-box state callback table.
 *
 * TextBox_BuildStep indexes this table with the low five bits of state byte
 * 0x51. The image contains exactly eighteen entries; the masked index can
 * exceed that bound, so the public declaration remains unsized.
 */

TextBoxStateCallback D_80090E64[] = {
    Dialog_UpdateChoice,
    Dialog_UpdateChoice,
    Text_CompletePageAdvance,
    Text_BeginPageClear,
    Text_WaitForPageAdvance,
    Text_StageScriptImage,
    Text_ContinueDisplayEffect,
    Text_WaitForDisplayEffectAndRelease,
    Text_WaitForDisplayEffect,
    Text_RequestDisplayEffectCrossfade,
    TextBox_WaitForScriptCompletion,
    Text_StartViewportTween,
    DuelEffect_WaitForFileTransfer,
    Text_WaitForBgmFade,
    Text_RequestDisplayEffectFadeOut,
    Text_WaitForDelay,
    Text_WaitWithViewportShake,
    Text_WaitForCdSoundTransfer,
};
