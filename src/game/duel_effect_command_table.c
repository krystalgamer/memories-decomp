#include "../types.h"
#include "duel_effect_command_table.h"

/* Initialized data at 0x80090EAC: the duel effect command table.
 *
 * func_80038B4C is a bytecode dispatcher. It reads one byte from the stream a
 * display effect record carries, advances that pointer past it, and calls the
 * entry it selects. The index is therefore whatever the stream holds rather
 * than a masked field, which is why the array stays unsized: its bound is a
 * property of the data, not of the call site.
 *
 * Forty-seven entries, all matching C, and almost all of them live in the
 * duel_effect_* sources, which is where the name comes from. */

void DuelEffect_PlaySoundCommand(u8 *);
void Text_HandleChoiceCommand(u8 *);
void Text_SetCursorOffset(u8 *);
void Text_StartCampaignDuel(u8 *);
void func_80037D2C(u8 *);
void func_80037D6C(u8 *);
void func_80037DA4(u8 *);
void func_80038070(u8 *);
void func_80038094(u8 *);
void func_800380D4(u8 *);
void func_80038110(u8 *);
void func_80038148(u8 *);
void func_800382A8(u8 *);
void func_80038334(u8 *);
void func_80038388(u8 *);
void func_800383B0(u8 *);
void func_800383DC(u8 *);
void func_80038498(u8 *);
void func_800384E4(u8 *);
void func_80038690(u8 *);
void func_800386B8(u8 *);
void func_80038800(u8 *);
void func_80038888(u8 *);
void func_80038890(u8 *);
void func_80038898(u8 *);
void func_800388D8(u8 *);
void func_800389C4(u8 *);
void func_800389D8(u8 *);
void func_80038A44(u8 *);
void func_80038AB0(u8 *);
void func_80038B08(u8 *);
void func_80038B4C(u8 *);
void func_80038D14(u8 *);
void func_80038D2C(u8 *);
void func_80038DB8(u8 *);
void func_80038E1C(u8 *);
void func_80038E7C(u8 *);
void func_80038EB0(u8 *);
void func_80039AFC(u8 *);
void func_80039BE0(u8 *);
void func_80039C94(u8 *);

void (*D_80090EAC[])(u8 *) = {
    func_80037DA4,
    func_800380D4,
    func_80038110,
    func_80038148,
    func_800382A8,
    func_80038334,
    func_80038388,
    func_800383B0,
    func_80038070,
    func_800383DC,
    func_80038498,
    func_80038094,
    func_800384E4,
    Text_StartCampaignDuel,
    func_80038690,
    func_800386B8,
    DuelEffect_PlaySoundCommand,
    func_80038800,
    func_80038888,
    func_80038890,
    func_80038898,
    func_800388D8,
    func_800389C4,
    func_800389D8,
    func_80038A44,
    func_80038AB0,
    func_80038B08,
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    func_80037D2C,
    func_80038EB0,
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
