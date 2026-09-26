#define GINPUT_PAD1_HELD_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#define SCRIPT_STATE_COMMAND_IN_DATA
#include "../types.h"
#include "text_box_wrap_line.h"
#include "input.h"
#include "duel_effect.h"
#include "duel_effect_state_callbacks.h"
#include "duel_effect_entry_control.h"
#include "duel_effect_entry_occupancy.h"
#include "text_constants.h"
#include "display_object_core.h"
#include "text_box_runtime.h"
#include "func_80036C14.h"
#include "menu_record_reset.h"
#include "script_command_table.h"
#include "script_state.h"
#include "text_box_state_callbacks.h"
#include "text_stream_commands.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what makes the
   opcode store below a single %gp_rel instruction whose load-delay slot needs
   the retail nop. The address comes from c_symbols.ld, which overrides this
   common symbol, so no storage is allocated here.  volatile is what keeps the
   read-back after the store, which retail issues at every use. */
volatile u16 D_8009B33A;

/* The glyph counter and its limit, and the buttons that skip ahead. The
   Japanese channel record is 0x60 bytes, so both fields sit two bytes lower
   there, and that release confirms with Circle rather than Cross. A regional
   build supplies its own. */
#ifndef TEXT_BOX_GLYPH_COUNT
#define TEXT_BOX_GLYPH_COUNT(o) ((o)->field_60)
#define TEXT_BOX_GLYPH_LIMIT(o) ((o)->field_61)
#define TEXT_BOX_RANGE_START(o) ((o)->range_start_5C)
#endif
#ifndef TEXT_BOX_ENTRY_TYPE
#define TEXT_BOX_ENTRY_TYPE DuelEffectEntry
#endif
#ifndef TEXT_BOX_ADVANCE_MASK
#define TEXT_BOX_ADVANCE_MASK PAD_BUTTON_CONFIRM_MASK
#endif

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
    if ((flags & TEXT_BOX_FLAG_BUILD_ACTIVE) == 0) {
        flags |= TEXT_BOX_FLAG_BUILD_ACTIVE;
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
        TEXT_BOX_GLYPH_COUNT(object) = 0;
        object->stream_58 = 0;
#ifdef VERSION_JAPAN
        /* The Japanese two-bank layout, as in its Text_LookupString. */
        if (id & TEXT_GLOBAL_STRING_ID_BASE) {
            text = (u8 *)(((u32)D_801D6000 & TEXT_BANK_ADDRESS_MASK) |
                D_801D6000[id & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) |
                D_801C0000[id]);
        }
#else
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
#endif
        object->text_00 = text;
        object->field_56 = 0;
        object->state_51 = 0;
        DisplayObject_ReleaseIfPresent(object->field_30);
        DisplayObject_ReleaseIfPresent(object->field_2C);
        object->field_30 = (void *)0;
        object->field_2C = 0;
        func_800391E4(object);
        if ((object->flags_34 & 0x40) == 0) {
            entry = (DuelEffectEntry *)&((TEXT_BOX_ENTRY_TYPE *)tent_DuelEffectEntries)[
                TEXT_BOX_RANGE_START(object)];
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
        D_80090E64[object->state_51 & DUEL_EFFECT_STATE_INDEX_MASK](object);
        object->flags_34 = object->flags_34 & 0xFBFF;
        return;
    }
    if ((object->flags_34 & 0x1C00) == 0) {
        if ((gInput_wPad1Held & PAD_BUTTON_SQUARE) ||
            (gInput_wPad1Pressed & TEXT_BOX_ADVANCE_MASK)) {
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
    slot = &((TextStreamOwner *)object)->streams[object->stream_58];
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
    if (TextBox_WrapLineIfNeeded(object) != 0) {
        object->state_51 = 4;
        return;
    }
    D_8009B35A = D_8009B33A;
    func_80036C14(object, tent_GlyphLookupTable[(s16)D_8009B33A] & 0x8FF0FFFF);
    TEXT_BOX_GLYPH_COUNT(object) = TEXT_BOX_GLYPH_COUNT(object) + 1;
    if (TEXT_BOX_GLYPH_LIMIT(object) != 0 &&
        TEXT_BOX_GLYPH_COUNT(object) >= TEXT_BOX_GLYPH_LIMIT(object)) {
        object->flags_34 = object->flags_34 | TEXT_BOX_FLAG_DONE;
    }
    object->field_38 = object->field_38 + object->field_5A;
}
