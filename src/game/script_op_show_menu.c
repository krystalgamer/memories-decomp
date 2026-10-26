#include "../types.h"
#include "display_effect_lifecycle.h"
#include "display_effect_process_menu_records.h"
#include "script_command_busy.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "menu_record_reset.h"
#include "script_state.h"
#include "script_op_show_menu.h"

void Script_OpShowMenu(void)
{
    MenuRecord *o;
    s32 first;
    s32 flags;
    s32 bit;

    if (ScriptCommand_MarkStarted() == 0) {
        first = *gScript_StreamCursor++;
        flags = *gScript_StreamCursor++;
        bit = flags & 0x80;

        o = &gDisplayEffect_aMenuRecords[bit >> 7];
        gScript_pMenuRecord = o;

        if (o->effect_id >= 0) {
            DisplayEffect_ReleaseRecord(o);
        }
        DisplayEffect_ResetMenuRecords();
        DisplayEffect_ResetRecordState((DisplayEffectState *)o);

        o->resource_entry_index = flags & 15;
        o->effect_id = first;
        o->slot_index = bit >> 7;
        o->display_effect_step = 2;
        if (bit) {
            o->field_34 = 0xD8;
        }
    }

    DisplayEffect_ProcessMenuRecords(0);
    if (gScript_pMenuRecord->display_effect_step == 0) {
        gScript_wCommandWord = 0;
    }
}
