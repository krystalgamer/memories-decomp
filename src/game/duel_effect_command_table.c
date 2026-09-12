#include "../types.h"
#include "duel_effect_command.h"
#include "duel_effect_command_table.h"
#include "text_control_commands.h"
#include "duel_effect_object_commands.h"
#include "duel_effect_script_dispatch.h"
#include "text_stream_commands.h"
#include "duel_effect_basic_commands.h"
#include "duel_effect_play_sound_command.h"
#include "../unmatched.h"
#include "text_start_campaign_duel.h"
#include "display_object_fade_callbacks.h"

/* Initialized data at 0x80090EAC: the duel effect command table.
 *
 * Text_DispatchSecondaryCommand is a bytecode dispatcher. It reads one byte
 * from the stream a display effect record carries, advances that pointer past
 * it, and calls the entry it selects. The index is therefore whatever the
 * stream holds rather than a masked field, which is why the array stays
 * unsized: its bound is a property of the data, not of the call site.
 *
 * Forty-seven entries, all matching C, and almost all of them live in the
 * duel_effect_* sources, which is where the name comes from.
 *
 * Every entry now comes through its unit's header in the include block
 * above. */

void (*D_80090EAC[])(DuelEffectChannel *) = {
    (void (*)(DuelEffectChannel *))func_80037DA4,
    func_800380D4,
    func_80038110,
    (void (*)(DuelEffectChannel *))func_80038148,
    (void (*)(DuelEffectChannel *))func_800382A8,
    func_80038334,
    func_80038388,
    func_800383B0,
    func_80038070,
    (void (*)(DuelEffectChannel *))func_800383DC,
    (void (*)(DuelEffectChannel *))func_80038498,
    func_80038094,
    (void (*)(DuelEffectChannel *))func_800384E4,
    Text_StartCampaignDuel,
    (void (*)(DuelEffectChannel *))func_80038690,
    func_800386B8,
    DuelEffect_PlaySoundCommand,
    (void (*)(DuelEffectChannel *))func_80038800,
    (void (*)(DuelEffectChannel *))func_80038888,
    (void (*)(DuelEffectChannel *))func_80038890,
    (void (*)(DuelEffectChannel *))func_80038898,
    (void (*)(DuelEffectChannel *))func_800388D8,
    func_800389C4,
    func_800389D8,
    func_80038A44,
    func_80038AB0,
    func_80038B08,
    (void (*)(DuelEffectChannel *))Text_ExtendGlyphCode,
    (void (*)(DuelEffectChannel *))Text_ExtendGlyphCode,
    (void (*)(DuelEffectChannel *))Text_ExtendGlyphCode,
    (void (*)(DuelEffectChannel *))Text_ExtendGlyphCode,
    (void (*)(DuelEffectChannel *))Text_ExtendGlyphCode,
    (void (*)(DuelEffectChannel *))Text_ExtendGlyphCode,
    (void (*)(DuelEffectChannel *))Text_HandleDisplayEffectCommand,
    (void (*)(DuelEffectChannel *))Text_SetStateFromStream,
    Text_DispatchSecondaryCommand,
    Text_HandleCampaignFlagCommand,
    Text_StartPageWait,
    Text_HandleChoiceCommand,
    Text_PushStreamOffset,
    Text_SetCursorOffset,
    Text_NewLine,
    Text_EndStream,
    func_80039BE0,
    func_80039AFC,
    func_80039BE0,
    func_80039C94,
};
