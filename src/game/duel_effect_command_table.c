#include "../types.h"
#include "duel_effect_command.h"
#include "duel_effect_command_table.h"
#include "text_control_commands.h"
#include "duel_effect_object_commands.h"
#include "duel_effect_script_dispatch.h"
#include "text_stream_commands.h"
#include "duel_effect_basic_commands.h"
#include "duel_effect_fade_command.h"
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

void (*D_80090EAC[])(u8 *) = {
    func_80037DA4,
    (void (*)(u8 *))func_800380D4,
    (void (*)(u8 *))func_80038110,
    func_80038148,
    func_800382A8,
    (void (*)(u8 *))func_80038334,
    (void (*)(u8 *))func_80038388,
    (void (*)(u8 *))func_800383B0,
    (void (*)(u8 *))func_80038070,
    (void (*)(u8 *))func_800383DC,
    func_80038498,
    (void (*)(u8 *))func_80038094,
    func_800384E4,
    (void (*)(u8 *))Text_StartCampaignDuel,
    (void (*)(u8 *))func_80038690,
    (void (*)(u8 *))func_800386B8,
    (void (*)(u8 *))DuelEffect_PlaySoundCommand,
    (void (*)(u8 *))func_80038800,
    (void (*)(u8 *))func_80038888,
    (void (*)(u8 *))func_80038890,
    func_80038898,
    func_800388D8,
    (void (*)(u8 *))func_800389C4,
    (void (*)(u8 *))func_800389D8,
    (void (*)(u8 *))func_80038A44,
    (void (*)(u8 *))func_80038AB0,
    (void (*)(u8 *))func_80038B08,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    Text_ExtendGlyphCode,
    (void (*)(u8 *))Text_HandleDisplayEffectCommand,
    Text_SetStateFromStream,
    Text_DispatchSecondaryCommand,
    (void (*)(u8 *))Text_HandleCampaignFlagCommand,
    (void (*)(u8 *))Text_StartPageWait,
    Text_HandleChoiceCommand,
    (void (*)(u8 *))Text_PushStreamOffset,
    (void (*)(u8 *))Text_SetCursorOffset,
    Text_NewLine,
    Text_EndStream,
    (void (*)(u8 *))func_80039BE0,
    (void (*)(u8 *))func_80039AFC,
    (void (*)(u8 *))func_80039BE0,
    (void (*)(u8 *))func_80039C94,
};
