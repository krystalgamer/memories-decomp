#include "../types.h"
#include "text_stream_commands.h"
#include "duel_effect.h"
#include "duel_effect_entry_occupancy.h"
#include "../unmatched.h"
#include "dialog_choice.h"
#include "text_handle_choice_command.h"

extern u8 gDialog_bInputState;

void Text_HandleChoiceCommand(u8 *p)
{
    s32 t;
    s32 u;
    s32 w;
    s32 v;
    s32 c;
    s32 d;

    D_8009B350 = 1;
    t = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
    c = t;
    d = 0xF;
    if (c & 8) {
        u = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
        t = u;
        d = t;
    }
    if (c & 0x80) {
        *(s32 *)(p - -(*(s8 *)(p + 0x58) * 4)) += gDialog_bChoice * 2;
        Text_SetCursorOffset(p);
    } else {
        gDialog_bChoiceCount = 7;
        gDialog_bChoiceCount = c & gDialog_bChoiceCount;
        D_8009B34C = c & 0xF0;
        gDialog_bChoiceEnabled = d & 0xF;
        w = d & 0x80;
        gDialog_bChoice = 0;
        gDialog_bInputState = 0;
        if (w != 0) {
            gDialog_bInputState = 1;
        }
        func_80035CA8(p[0x57]);
        DuelEffect_ClearMatchingMarker(p[0x57]);
        v = *(u16 *)(p + 0x34);
        p[0x56] = 0;
        D_8009B340 = func_80037CE0;
        *(u16 *)(p + 0x34) = v | 0x1000;
    }
}
