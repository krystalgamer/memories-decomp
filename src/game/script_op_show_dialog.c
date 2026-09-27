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

/* Regional values, which the European wrapper defines for itself: the
   dialog box's height (0x30 in the US build, 0x40 in the European one)
   and the flag set on it while the command runs (8 in the US build, 0x10
   in the European one). */
#ifndef SCRIPT_DIALOG_BOX_HEIGHT
#define SCRIPT_DIALOG_BOX_HEIGHT 0x30
#endif
#ifndef SCRIPT_DIALOG_BOX_FLAG
#define SCRIPT_DIALOG_BOX_FLAG 8
#endif

void Script_OpShowDialog(void)
{
    u8 *script;
    DuelEffectChannel *box;
    s32 value;
    u16 flags;
    u16 boxflags;

    if (ScriptCommand_MarkStarted() == 0) {
        script = D_8009B290;
        D_8009B290 = script + 2;
        value = script[0] | (script[1] << 8);
        D_8009B2A4 |= 0x4000;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        /* The Japanese and European builds make no func_8003B6AC call here. */
        func_8003B6AC(0, 2);
#endif
        box = TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, SCRIPT_DIALOG_BOX_HEIGHT);
        DuelEffect_MarkObjectIfActive((MenuRecord *)box);
        box->flags_34 |= SCRIPT_DIALOG_BOX_FLAG;
        if ((value & 0x8000) != 0) {
            flags = D_8009B27C;
            boxflags = *(volatile u16 *)&box->flags_34;
            D_8009B27C = flags | 0x4000;
            box->flags_34 = boxflags & (0xFFFF ^ SCRIPT_DIALOG_BOX_FLAG);
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
