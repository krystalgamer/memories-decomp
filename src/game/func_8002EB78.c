#include "../types.h"
#include "display_effect_lifecycle.h"
#include "display_effect_process_menu_records.h"
#include "script_command_busy.h"
#include "menu_record.h"
#include "menu_record_reset.h"
#include "script_state.h"
#include "func_8002EB78.h"

void func_8002EB78(void)
{
    MenuRecord *o;
    s32 first;
    s32 flags;
    s32 bit;

    if (func_8002E3B4() == 0) {
        first = *D_8009B290++;
        flags = *D_8009B290++;
        bit = flags & 0x80;

        o = &D_800EB010[bit >> 7];
        D_8009B274 = o;

        if (o->field_30 >= 0) {
            func_80039FD4((u8 *)o);
        }
        func_80039E9C();
        func_80039F44((DisplayEffectState *)o);

        o->field_31 = flags & 15;
        o->field_30 = first;
        o->field_3C = bit >> 7;
        o->display_effect_step = 2;
        if (bit) {
            o->field_34 = 0xD8;
        }
    }

    DisplayEffect_ProcessMenuRecords(0);
    if (D_8009B274->display_effect_step == 0) {
        D_8009B27C = 0;
    }
}
