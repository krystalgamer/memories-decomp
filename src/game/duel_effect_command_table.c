#include "../types.h"
#include "duel_effect_command.h"
#include "duel_effect_command_table.h"
#include "duel_effect_completion.h"
#include "duel_effect_object_commands.h"
#include "duel_effect_script_dispatch.h"
#include "duel_effect_stream_fields.h"
#include "text_stream_commands.h"
#include "duel_effect_basic_commands.h"
#include "duel_effect_play_sound_command.h"
#include "func_80037DA4.h"
#include "func_80038148.h"
#include "func_800382A8.h"
#include "func_800383DC.h"
#include "func_80038498.h"
#include "func_800384E4.h"
#include "func_800388D8.h"
#include "func_80038B4C.h"
#include "func_80038DB8.h"
#include "func_80038EB0.h"
#include "text_control_commands.h"
#include "text_handle_choice_command.h"
#include "text_start_campaign_duel.h"

/* Initialized data at 0x80090EAC: the duel effect command table.
 *
 * func_80038B4C is a bytecode dispatcher. It reads one byte from the stream a
 * display effect record carries, advances that pointer past it, and calls the
 * entry it selects. The index is therefore whatever the stream holds rather
 * than a masked field, which is why the array stays unsized: its bound is a
 * property of the data, not of the call site.
 *
 * Forty-seven entries, all matching C, and almost all of them live in the
 * duel_effect_* sources, which is where the name comes from.
 *
 * The prototypes still written out below belong to units that have no header
 * yet. Each one that has gained a header comes through the include above
 * instead, and this block shrinks as the rest follow. */

void func_80039AFC(u8 *);
void func_80039BE0(u8 *);
void func_80039C94(u8 *);

void (*D_80090EAC[])(u8 *) = {
    func_80037DA4,
    func_800380D4,
    func_80038110,
    func_80038148,
    func_800382A8,
    (void (*)(u8 *))func_80038334,
    (void (*)(u8 *))func_80038388,
    (void (*)(u8 *))func_800383B0,
    (void (*)(u8 *))func_80038070,
    (void (*)(u8 *))func_800383DC,
    func_80038498,
    func_80038094,
    func_800384E4,
    Text_StartCampaignDuel,
    (void (*)(u8 *))func_80038690,
    func_800386B8,
    DuelEffect_PlaySoundCommand,
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
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    (void (*)(u8 *))func_80038EB0,
    func_80037D6C,
    func_80038B4C,
    func_80038D2C,
    func_80038D14,
    Text_HandleChoiceCommand,
    func_80038DB8,
    Text_SetCursorOffset,
    func_80038E1C,
    func_80038E7C,
    func_80039BE0,
    func_80039AFC,
    func_80039BE0,
    func_80039C94,
};
