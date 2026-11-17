#include "../types.h"
#include "duel_effect_text.h"
#include "duel_effect_command_table.h"
#include "text_stream_commands.h"
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
 * calling convention: the primary handlers all take the channel, while the
 * no-argument secondary no-ops, Text_PushStagedString's unread return and the
 * one-argument entry handlers (called with the entry and its channel) are
 * cast to their table's type.
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
    DuelEffect_FadeOutBgmCommand,
    (void (*)(DuelEffectChannel *))Text_NoOpSecondary12,
    (void (*)(DuelEffectChannel *))Text_NoOpSecondary13,
    Text_OpenCampaignMap,
    Text_ApplyFadeCommand,
    Text_DisableCompletionWait,
    Text_JumpBySelectorA,
    Text_JumpBySelectorB,
    Text_UnlockDuelist,
    Text_CloseChoice,
#ifdef VERSION_EUROPE
    Text_PushPlayerNameStream,
    Text_SetPenY,
#endif
};

void (*gText_apfnPrimaryCommands[])(DuelEffectChannel *) = {
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_HandleDisplayEffectCommand,
    Text_SetStateFromStream,
    Text_DispatchSecondaryCommand,
    Text_HandleCampaignFlagCommand,
    Text_StartPageWait,
    Text_HandleChoiceCommand,
    Text_PushStreamOffset,
    Text_SetCursorOffset,
    Text_NewLine,
    Text_EndStream,
};

void (*gDuelEffect_EntryHandlers[])(DuelEffectEntry *, DuelEffectChannel *) = {
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphConceal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphReveal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphConceal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateDelayedGlyphRotation,
#ifdef VERSION_EUROPE
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphSpiralFade,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_StartGlyphSpiralReveal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_StartGlyphSpiralConceal,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphLinearFade,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphFadeIn,
    (void (*)(DuelEffectEntry *, DuelEffectChannel *))DuelEffect_UpdateGlyphFadeOut,
#endif
};
