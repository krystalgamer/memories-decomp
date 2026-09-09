#include "../types.h"
#include "display_effect_lifecycle.h"
#include "script_command_busy.h"
#include "menu_record.h"
#include "script_state.h"

extern u8 *D_8009B290;
extern u8 *D_8009B274;
extern s8 *func_80039E9C(void);
extern void func_8003B50C(s32);

void func_8002EB78(void)
{
    u8 *o;
    s32 first;
    s32 flags;
    s32 bit;

    if (func_8002E3B4() == 0) {
        first = *D_8009B290++;
        flags = *D_8009B290++;
        bit = flags & 0x80;

        o = (u8 *)D_800EB010 + ((bit >> 7) * sizeof(MenuRecord));
        D_8009B274 = o;

        if (*(s8 *)(o + 0x30) >= 0) {
            func_80039FD4((u8 *)o);
        }
        func_80039E9C();
        func_80039F44((DisplayEffectState *)o);

        o[0x31] = flags & 15;
        o[0x30] = first;
        o[0x3C] = bit >> 7;
        o[0x33] = 2;
        if (bit) {
            *(s16 *)(o + 0x34) = 0xD8;
        }
    }

    func_8003B50C(0);
    if (D_8009B274[0x33] == 0) {
        D_8009B27C = 0;
    }
}
