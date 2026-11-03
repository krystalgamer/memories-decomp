#include "../types.h"
#include "secondary_scene_commands.h"
#include "duel_effect_command.h"
#include "duel_effect_command_table.h"
#include "text_control_commands.h"
#include "duel_effect_object_commands.h"
#include "duel_effect_fade_out_bgm_command.h"
#include "text_stream_commands.h"
#include "duel_effect_sound_commands.h"
#include "duel_effect_play_sound_command.h"
#include "../unmatched.h"
#include "text_start_campaign_duel.h"
#include "display_object_fade_callbacks.h"
#include "text_channel_state.h"
#include "duel_effect_process_entries.h"

/* Initialized data at 0x80090EAC: three adjacent dispatch tables.
 *
 * Text_DispatchSecondaryCommand is a bytecode dispatcher. It reads one byte
 * from the stream a display effect record carries, advances that pointer past
 * it, and calls the entry it selects. The index is therefore whatever the
 * stream holds rather than a masked field, which is why the array stays
 * unsized: its bound is a property of the data, not of the call site.
 * NA and Japan define twenty-seven secondary commands; Europe has twenty-nine.
 *
 * TextBox_BuildStep dispatches stream bytes 0xF0 through 0xFF through the
 * sixteen primary commands that follow, and DuelEffect_ProcessEntries selects
 * one of the four entry handlers after those. Each table keeps its consumer's
 * calling convention, so the shared handlers are cast to it.
 *
 * Every entry now comes through its unit's header in the include block
 * above. */

void (*gText_SecondaryCommandHandlers[])(DuelEffectChannel *) = {
    Text_ExpandSelectedCardInfo,
    Text_ResetPenXAndAdvanceY,
    Text_AdvancePenX,
    Text_PushFormattedDecimal,
    Text_SetFontMode,
    Text_SetGlyphCellSize,
    Text_SetPenX,
    Text_SetOutputLimit,
    Text_EmitGuardianStarGlyph,
    (void (*)(DuelEffectChannel *))Text_PushStagedString,
    Text_SetColorIndex,
    Text_EmitNextSymbolGlyph,
    Text_SetFastPrintMode,
    Text_StartCampaignDuel,
    DuelEffect_PlayBgmCommand,
    DuelEffect_ProcessBgmCommand,
    DuelEffect_PlaySoundCommand,
    (void (*)(DuelEffectChannel *))DuelEffect_FadeOutBgmCommand,
    (void (*)(DuelEffectChannel *))Text_NoOpSecondary12,
    (void (*)(DuelEffectChannel *))Text_NoOpSecondary13,
    Text_OpenCampaignMap,
    func_800388D8,
    Text_DisableCompletionWait,
    Text_JumpBySelectorA,
    Text_JumpBySelectorB,
    Text_UnlockDuelist,
    Text_CloseChoice,
};

void (*gText_apfnPrimaryCommands[])(u8 *) = {
    (void (*)(u8 *))Text_ExtendGlyphCode,
    (void (*)(u8 *))Text_ExtendGlyphCode,
    (void (*)(u8 *))Text_ExtendGlyphCode,
    (void (*)(u8 *))Text_ExtendGlyphCode,
    (void (*)(u8 *))Text_ExtendGlyphCode,
    (void (*)(u8 *))Text_ExtendGlyphCode,
    (void (*)(u8 *))Text_HandleDisplayEffectCommand,
    (void (*)(u8 *))Text_SetStateFromStream,
    (void (*)(u8 *))Text_DispatchSecondaryCommand,
    (void (*)(u8 *))Text_HandleCampaignFlagCommand,
    (void (*)(u8 *))Text_StartPageWait,
    (void (*)(u8 *))Text_HandleChoiceCommand,
    (void (*)(u8 *))Text_PushStreamOffset,
    (void (*)(u8 *))Text_SetCursorOffset,
    (void (*)(u8 *))Text_NewLine,
    (void (*)(u8 *))Text_EndStream,
};

void (*gDuelEffect_EntryHandlers[])(DuelEffectEntry *, DuelEffectChannel *) = {
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphConceal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphReveal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphConceal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateDelayedGlyphRotation,
};
