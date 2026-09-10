#define GINPUT_PAD1_HELD_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#include "../types.h"
#include "func_80037C74.h"
#include "input.h"
#include "duel_effect.h"
#include "duel_effect_interaction_states.h"
#include "duel_effect_entry_control.h"
#include "duel_effect_entry_occupancy.h"
#include "text_constants.h"
#include "display_object_api.h"
#include "text_box_runtime.h"
#include "func_80036C14.h"
#include "menu_record_reset.h"
#include "script_command_table.h"
#include "text_box_state_callbacks.h"
#include "text_stream_commands.h"

extern void (*D_80090F18[])(u8 *);

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what makes the
   opcode store below a single %gp_rel instruction whose load-delay slot needs
   the retail nop. The address comes from c_symbols.ld, which overrides this
   common symbol, so no storage is allocated here.  volatile is what keeps the
   read-back after the store, which retail issues at every use. */
volatile u16 D_8009B33A;
extern u16 D_8009B27C __attribute__((section(".data")));

void TextBox_BuildStep(DuelEffectChannel *object)
{
    u16 flags;
    s32 id;
    u8 *text;
    u8 **slot;
    u8 *script;
    DuelEffectEntry *entry;
    s32 op;
    void (**handlers)(u8 *);

    flags = object->flags_34;
    if ((flags & 0x4000) == 0) {
        flags |= 0x4000;
        object->flags_34 = flags;
        if ((flags & 2) == 0) {
            func_80039E9C();
        }
        if ((object->flags_34 & 0x100) != 0) {
            object->field_5B = 8;
            object->field_5A = 8;
        }
        id = object->field_36;
        D_8009B357 = 0;
        D_8009B340 = 0;
        object->delay_52 = 1;
        object->field_60 = 0;
        object->stream_58 = 0;
        if (id > 0xCFFF) {
            text = (u8 *)(((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) +
                D_801C0000[id - 0xD000]);
        } else if (id > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
            text = (u8 *)(((u32)D_801D5800 & TEXT_BANK_ADDRESS_MASK) +
                D_801D5800[id - TEXT_GLOBAL_STRING_ID_BASE]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)D_801B0000 & TEXT_BANK_ADDRESS_MASK) +
                D_801C0000[id]);
        }
        object->text_00 = text;
        object->field_56 = 0;
        object->state_51 = 0;
        func_8004036C(object->field_30);
        func_8004036C((void *)object->field_2C);
        object->field_30 = (void *)0;
        object->field_2C = 0;
        func_800391E4(object);
        if ((object->flags_34 & 0x40) == 0) {
            entry = &D_800EB288[object->range_start_5C];
            object->entry_head_24 = entry;
            object->entry_end_20 = entry;
            func_80035CA8(object->index_57);
            DuelEffect_ClearMatchingMarker(object->index_57);
        }
        return;
    }

    if (D_8009B357 != 0) {
        D_80090C50[*(u8 *)&D_8009B27C]();
        if (D_8009B27C == 0) {
            D_8009B357 = 0;
        }
    }
    if (object->state_51 != 0) {
        D_80090E64[object->state_51 & 0x1F](object);
        object->flags_34 = object->flags_34 & 0xFBFF;
        return;
    }
    if ((object->flags_34 & 0x1C00) == 0) {
        if ((gInput_wPad1Held & 0x80) || (gInput_wPad1Pressed & 0xC0)) {
            func_800373C8(object, 0, 0);
            object->delay_52 = 1;
            object->flags_34 = object->flags_34 | 0x400;
        }
        object->delay_52 = object->delay_52 - 1;
        if (object->delay_52 != 0) {
            return;
        }
    }
    handlers = D_80090F18;
    object->delay_52 = object->field_53;
next_opcode:
    slot = (u8 **)((u8 *)object + object->stream_58 * 4);
    script = *slot;
    D_8009B33A = script[0];
    op = (s16)D_8009B33A;
    *slot = script + 1;
    if (op >= 0xF0) {
        D_8009B350 = 0;
        handlers[(s16)D_8009B33A - 0xF0]((u8 *)object);
        if (D_8009B350 >= 0) {
            if (D_8009B350 == 1) {
                return;
            }
            goto next_opcode;
        }
    }
    if (func_80037C74(object) != 0) {
        object->state_51 = 4;
        return;
    }
    D_8009B35A = D_8009B33A;
    func_80036C14(object, D_801D9000[(s16)D_8009B33A] & 0x8FF0FFFF);
    object->field_60 = object->field_60 + 1;
    if (object->field_61 != 0 && object->field_60 >= object->field_61) {
        object->flags_34 = object->flags_34 | 0x2000;
    }
    object->field_38 = object->field_38 + object->field_5A;
}
