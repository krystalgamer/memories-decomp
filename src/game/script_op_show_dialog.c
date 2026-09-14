#include "../types.h"
#include "script_command_busy.h"
#include "duel_effect.h"
#include "func_8003B6AC.h"
#include "text_box_lifecycle.h"
#include "script_state.h"
#include "script_op_show_dialog.h"
#include "duel_effect_mark_object_if_active.h"

/* Defined rather than declared: the assembler only resolves a small
   global gp-relative when the translation unit defines it, which is what
   makes the store below a single %gp_rel instruction whose load-delay
   slot carries the retail nop. c_symbols.ld overrides this common symbol,
   so no storage is allocated here. */
u16 D_8009B28C;

void Script_OpShowDialog(void)
{
    u8 *script;
    DuelEffectChannel *box;
    s32 value;
    u16 flags;
    u16 boxflags;

    if (func_8002E3B4() == 0) {
        script = D_8009B290;
        D_8009B290 = script + 2;
        value = script[0] | (script[1] << 8);
        D_8009B2A4 |= 0x4000;
        func_8003B6AC(0, 2);
        box = TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, 0x30);
        DuelEffect_MarkObjectIfActive((MenuRecord *)box);
        box->flags_34 |= 8;
        if ((value & 0x8000) != 0) {
            flags = D_8009B27C;
            boxflags = *(volatile u16 *)&box->flags_34;
            D_8009B27C = flags | 0x4000;
            box->flags_34 = boxflags & 0xFFF7;
        }
        D_8009B28C = D_8009B27C;
    } else {
        if ((D_8009B2A4 & 0x4000) == 0) {
            if ((D_8009B27C & 0x4000) == 0) {
                TextBox_Destroy(D_800EB0F8);
            }
            D_8009B28C = 0;
            D_8009B27C = 0;
        }
    }
}
