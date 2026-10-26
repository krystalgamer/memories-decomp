#define GINPUT_PAD1_HELD_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#define SCRIPT_STATE_COMMAND_IN_DATA
#include "../types.h"
#include "text_box_wrap_line.h"
#include "input.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_state_callbacks.h"
#include "duel_effect_entry_control.h"
#include "duel_effect_entry_occupancy.h"
#include "text_constants.h"
#include "text_glyph_lookup.h"
#include "text_string_bank_views.h"
#include "european_text_bank_views.h"
#include "display_object_core.h"
#include "text_box_runtime.h"
#include "duel_effect_entries.h"
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
volatile u16 gText_CurrentStreamCode;

/* The glyph counter and its limit, and the buttons that skip ahead. The
   Japanese channel record is 0x60 bytes, so both fields sit two bytes lower
   there, and that release confirms with Circle rather than Cross. A regional
   build supplies its own. */
#ifndef TEXT_BOX_GLYPH_COUNT
#define TEXT_BOX_GLYPH_COUNT(o) ((o)->output_count)
#define TEXT_BOX_GLYPH_LIMIT(o) ((o)->output_limit)
#define TEXT_BOX_RANGE_START(o) ((o)->entry_range_start)
#endif
#ifndef TEXT_BOX_ENTRY_TYPE
#define TEXT_BOX_ENTRY_TYPE DuelEffectEntry
#endif
#ifndef TEXT_BOX_ADVANCE_MASK
#define TEXT_BOX_ADVANCE_MASK PAD_BUTTON_CONFIRM_MASK
#endif

/* The flag that keeps the box's previous contents: 2 in the US build, 4 in
   the European one, whose wrapper defines its own. */
#ifndef TEXT_BOX_BUILD_KEEP_FLAG
#define TEXT_BOX_BUILD_KEEP_FLAG 2
#endif
/* The flag that keeps the channel's entries: 0x40, 0x80 in the European
   build. */
#ifndef TEXT_BOX_BUILD_OWN_ENTRIES_FLAG
#define TEXT_BOX_BUILD_OWN_ENTRIES_FLAG 0x40
#endif

void TextBox_BuildStep(DuelEffectChannel *object)
{
    u16 flags;
    s32 id;
    u8 *text;
    u8 *G32 *slot;
    u8 *script;
    DuelEffectEntry *entry;
    s32 op;
    void (*G32 *handlers)(u8 *);

    flags = object->flags_34;
    if ((flags & TEXT_BOX_FLAG_BUILD_ACTIVE) == 0) {
        flags |= TEXT_BOX_FLAG_BUILD_ACTIVE;
        object->flags_34 = flags;
        if ((flags & TEXT_BOX_BUILD_KEEP_FLAG) == 0) {
            DisplayEffect_ResetMenuRecords();
        }
#ifndef VERSION_EUROPE
        /* The European build makes neither field store. */
        if ((object->flags_34 & 0x100) != 0) {
            object->line_advance_y = 8;
            object->glyph_advance_x = 8;
        }
#endif
        id = object->initial_string_id;
        gText_ScriptDispatchGate = 0;
        gText_ChoiceLayoutCallback = 0;
        object->delay_countdown = 1;
        TEXT_BOX_GLYPH_COUNT(object) = 0;
        object->stream_58 = 0;
#ifdef VERSION_JAPAN
        /* The Japanese two-bank layout, as in its Text_LookupString. */
        if (id & TEXT_GLOBAL_STRING_ID_BASE) {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                gText_GlobalStringOffsets[id & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                gText_PrimaryStringOffsets[id]);
        }
#elif defined(VERSION_EUROPE)
        /* The European banks: global strings from 0x8000, ids below 0x500
           in the first bank and the rest in the second, all through the
           tables four bytes into each bank. */
        if (id <= 0x7FFF) {
            if (id >= 0x500) {
                s32 n = id - 0x100;

                text = (u8 *)(((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                    gText_PrimaryStringOffsets[n]);
            } else {
                text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                    gText_PrimaryStringOffsets[id]);
            }
        } else {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_GlobalStringOffsets[id - 0x8000]);
        }
#else
        if (id > 0xCFFF) {
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_PrimaryStringOffsets[id - 0xD000]);
        } else if (id > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_GlobalStringOffsets[id - TEXT_GLOBAL_STRING_ID_BASE]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)gText_PrimaryStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                gText_PrimaryStringOffsets[id]);
        }
#endif
        object->text_00 = text;
        object->explicit_newline_count = 0;
        object->playback_state = 0;
        DisplayObject_ReleaseIfPresent(object->prompt_object);
        DisplayObject_ReleaseIfPresent(object->aux_object);
        object->prompt_object = (void *)0;
        object->aux_object = 0;
        TextBox_RefreshDisplayObjects(object);
        if ((object->flags_34 & TEXT_BOX_BUILD_OWN_ENTRIES_FLAG) == 0) {
            entry = (DuelEffectEntry *)&((TEXT_BOX_ENTRY_TYPE *)gDuelEffect_aEntries)[
                TEXT_BOX_RANGE_START(object)];
            object->entry_begin = entry;
            object->entry_write_cursor = entry;
#ifndef VERSION_EUROPE
            /* The European build no longer clears the channel's occupancy
               value, as in TextBox_Destroy. */
            DuelEffect_ClearOccupancyValue(object->channel_index);
#endif
            DuelEffect_ClearMatchingMarker(object->channel_index);
        }
        return;
    }

    if (gText_ScriptDispatchGate != 0) {
        gScript_apfnCommandHandlers[*(u8 *)&gScript_wCommandWord]();
        if (gScript_wCommandWord == 0) {
            gText_ScriptDispatchGate = 0;
        }
    }
    if (object->playback_state != 0) {
        gTextBox_apfnStateCallbacks[object->playback_state & DUEL_EFFECT_STATE_INDEX_MASK](object);
        object->flags_34 = object->flags_34 & 0xFBFF;
        return;
    }
    if ((object->flags_34 & 0x1C00) == 0) {
        if ((gInput_wPad1Held & PAD_BUTTON_SQUARE) ||
            (gInput_wPad1Pressed & TEXT_BOX_ADVANCE_MASK)) {
            DuelEffect_SetEntryControlBytes(object, 0, 0);
            object->delay_countdown = 1;
            object->flags_34 = object->flags_34 | 0x400;
        }
        object->delay_countdown = object->delay_countdown - 1;
        if (object->delay_countdown != 0) {
            return;
        }
    }
    handlers = gText_apfnPrimaryCommands;
    object->delay_countdown = object->glyph_delay;
next_opcode:
    slot = &((TextStreamOwner *)object)->streams[object->stream_58];
    script = *slot;
    gText_CurrentStreamCode = script[0];
    op = (s16)gText_CurrentStreamCode;
    *slot = script + 1;
    if (op >= 0xF0) {
        gText_CommandDisposition = 0;
        CALL32(void (*)(u8 *), handlers[(s16)gText_CurrentStreamCode - 0xF0])((u8 *)object);
        if (gText_CommandDisposition >= 0) {
            if (gText_CommandDisposition == 1) {
                return;
            }
            goto next_opcode;
        }
    }
    if (TextBox_WrapLineIfNeeded(object) != 0) {
        object->playback_state = 4;
        return;
    }
    gText_PumpedGlyphIndex = gText_CurrentStreamCode;
#ifdef VERSION_EUROPE
    /* The European glyph call returns a signed adjustment to the nominal
       horizontal advance. The box counts pixels (the wrapper makes the
       counter and its limit halfwords at 0x60 and 0x62) rather than
       glyphs. */
    {
        s32 w;
        s32 n;
        s32 limit;

        w = object->glyph_advance_x +
            DuelEffect_AppendEntry(object, gText_aGlyphLookup[(s16)gText_CurrentStreamCode].packed & 0x8FF0FFFF);
        limit = (s16)TEXT_BOX_GLYPH_LIMIT(object);
        object->pen_x = object->pen_x + w;
        n = TEXT_BOX_GLYPH_COUNT(object) + w;
        TEXT_BOX_GLYPH_COUNT(object) = n;
        if (limit != 0) {
            w = (s16)n + object->glyph_advance_x;
            if (limit < w) {
                object->flags_34 = object->flags_34 | TEXT_BOX_FLAG_DONE;
            }
        }
    }
#else
    DuelEffect_AppendEntry(object, gText_aGlyphLookup[(s16)gText_CurrentStreamCode].packed & 0x8FF0FFFF);
    TEXT_BOX_GLYPH_COUNT(object) = TEXT_BOX_GLYPH_COUNT(object) + 1;
    if (TEXT_BOX_GLYPH_LIMIT(object) != 0 &&
        TEXT_BOX_GLYPH_COUNT(object) >= TEXT_BOX_GLYPH_LIMIT(object)) {
        object->flags_34 = object->flags_34 | TEXT_BOX_FLAG_DONE;
    }
    object->pen_x = object->pen_x + object->glyph_advance_x;
#endif
}
