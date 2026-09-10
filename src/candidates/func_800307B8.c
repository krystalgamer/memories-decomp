/* Reclassified from matching_c (#3859). This was src/game/func_800307B8.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g4_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G4). Under
 * gcc_2_8_1_g8, a single threshold, it is 117 instructions against the
 * target's 120, opcode distance 3. The source below is the match, unchanged
 * apart from its include paths. */
#define GINPUT_PAD1_PRESSED_SIZED_VOLATILE
#define FRONTEND_DEBUG_SOUND_ROW_VIEW
#include "../types.h"
#include "../game/frontend_debug_tables.h"
#include "../game/display_object_brightness.h"
#include "../game/duel_interface_setup.h"
#include "../game/file_transfer.h"
#include "../game/input.h"
#include "../game/sound.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_voice_selection.h"
#include "../unmatched.h"

void func_800307B8(void)
{
    s32 flags;
    s32 count;
    s32 result;

    flags = D_8009B2EB;
    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        count = 3;
        func_80030090();
        gDebug_nSceneOrSoundID = gDebug_nLastSoundID[0];
        D_8009B2CA = D_8009B2DA;
        D_8009B2CC = gDebug_nLastSoundID[1];
        func_80030250(D_80090CB4, 0x11, 0x19, 0x21, 9, 4, count);
        D_8009B2C2 = count;
        D_8009B2C1 = count;
        return;
    }

    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_START) != 0) {
        SD_StopAll();
        return;
    }

    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_SELECT) != 0) {
        func_80014FA4();
        return;
    }

    result = func_80030294();
    if (result == 0) {
        return;
    }
    if (result < 0) {
        D_8009B2EB = 0;
        func_800300AC();
        return;
    }

    switch (*(s8 *)&D_8009B2DC) {
    case 0:
        gDebug_nLastSoundID[0] = gDebug_nSceneOrSoundID;
        if (*(s8 *)&D_8009B2E9 == 3) {
            func_8004763C();
            func_80047AD0((u16)((s16)gDebug_nLastSoundID[0] >> 12));
            return;
        }
        SD_SEPlayFull(gDebug_nSceneOrSoundID & 0xFFF);
        return;
    case 1:
        D_8009B2DA = D_8009B2CA;
        SD_BGMPlay(D_8009B2CA);
        return;
    case 2:
        gDebug_nLastSoundID[1] = D_8009B2CC;
        if ((gInput_wPad1Pressed[0] & PAD_BUTTON_SQUARE) != 0) {
            func_8003FFB4((u16)D_8009B2CC);
            return;
        }
        func_8003FF88(D_8009B2CC);
        return;
    }
}
