#ifndef MEMORIES_DECOMP_TEXT_CONSTANTS_H
#define MEMORIES_DECOMP_TEXT_CONSTANTS_H

#include "../types.h"

#define TEXT_GLOBAL_STRING_ID_BASE 0x8000
#ifdef VERSION_JAPAN_TEXT_LOOKUP_STRING
#define TEXT_BANK_ADDRESS_MASK 0x801F0000
#else
#define TEXT_BANK_ADDRESS_MASK 0xFFFF0000
#endif
#define TEXT_SINGLE_BYTE_GLYPH_LIMIT 0xF0
#define TEXT_STRING_TERMINATOR 0xFF
#define TEXT_DECIMAL_RADIX 10
#define TEXT_DECIMAL_BLANK_DIGIT 0xA

/* Resolves the existing bank address as a byte string. Both word arguments
 * remain; the current implementation does not use the first one. */
u8 *Text_LookupString(s32 unused_word, s32 string_id);

typedef union {
    u32 packed;
    struct {
        u16 key;
        u16 upper;
    } halves;
} TextGlyphLookupEntry;

typedef char TextGlyphLookupEntry_size_must_be_4[
    sizeof(TextGlyphLookupEntry) == 4 ? 1 : -1
];
typedef char TextGlyphLookupEntry_alignment_must_match_words[
    __alignof__(TextGlyphLookupEntry) == __alignof__(u32) ? 1 : -1
];
typedef char TextGlyphLookupEntry_key_offset_must_be_0[
    (u32)&(((TextGlyphLookupEntry *)0)->halves.key) == 0 ? 1 : -1
];
typedef char TextGlyphLookupEntry_upper_offset_must_be_2[
    (u32)&(((TextGlyphLookupEntry *)0)->halves.upper) == 2 ? 1 : -1
];

/* 0x8009B32E, the string id Text_PushStagedString resolves through the regional banks
 * above. It is two bytes: gDuelEffect_nCompactionRequested
 * (duel_effect_entry_state.h) starts at +2.
 * North America loads it unsigned, while Japan loads it signed before
 * testing bit 15 and masking the bank index. FreeDuel_PlaceCursor stores
 * into it through a named
 * address local, `slot = &gText_StagedStringId;` (screen_runtime.c:129-131), and
 * DuelScene_UpdateResultRewards stores a halfword to it through $at
 * (DuelScene_UpdateResultRewards.s:61-62). the-game.md:1511 calls it string ID 0x8328 + i.
 * The result controller selects the absolute arm for that store; the
 * existing readers keep the plain GP-relative declaration. */
extern u16 gText_StagedStringId;

/* The text colour slots, indexed by the low nibble of a colour command:
 * Text_SetColorIndex reads `gText_abColorSlots[v & 0xF]`.
 * MainMenu_CreateTradeDecisionMenu sets the first three to 4 and clears one
 * chosen by its argument, and
 * Options_InitTextDisplay in options_screen.c walks the table from its base.
 *
 * The incomplete-array spelling every consumer already used is kept. Nothing
 * here establishes how many slots there are -- the nibble index allows
 * sixteen, but only three are ever written by name -- so no bound is claimed,
 * for the same reason the banks above carry none. */
extern u8 gText_abColorSlots[];

/* The Shift-JIS key table: four-byte entries whose low halfword is the key.
 * Text_SjisToGlyphCodes (text_sjis_to_glyph_codes.c) walks it from entry 1
 * for a non-zero key, compares the low halfword against each source
 * halfword, stops at a zero word, and emits the entry index as the glyph
 * code; TextBox_BuildStep (text_box_build_step.c) reads packed at a
 * script byte's index and masks it with 0x8FF0FFFF.
 * TextBox_UpdateChannels (text_box_runtime.c) reads bits 0-2 of halves.upper;
 * text_init_decimal_digit_glyph_map.c reaches entry 1 under its own name,
 * gText_aGlyphSearchEntries, with a const that its note explains. Spelling
 * that as gText_aGlyphLookup + 1 (cast to const, or through a const arm of
 * this declaration) does not match: GCC 2.8.1 treats symbol+4 differently
 * from a separate symbol, which swaps t3/t4 or adds a register move (+8
 * bytes). The separate label is a measured exception. 0x174 bytes to gText_aGlyphRecordBitmaps,
 * 93 entries; the first word is zero in the image and no C unit writes the table.
 * Retail reaches it through %hi/%lo at every
 * site, which an incomplete array gives at any -G, so no unit needs an arm. */
extern TextGlyphLookupEntry gText_aGlyphLookup[];

/* The decimal digit glyph map. NA/Japan initialize lookup indices from the
 * copied Shift-JIS keys; Europe decodes glyph codes from a terminated string.
 * Preserve both original partial/empty initialization behaviors.
 * password/shop.c then reads it as `gText_DecimalDigitGlyphMap[gPassword_abDigits[i]]` to
 * turn an entered digit into a glyph, and Text_PushFormattedDecimal reads element 0 and
 * one chosen by a buffer byte.
 *
 * The incomplete-array spelling all three consumers already used is kept, and
 * for the usual reason: nothing here fixes the length. Text_PushFormattedDecimal writes
 * its reads as `*(u16 *)&gText_DecimalDigitGlyphMap[i]` even though the elements are already
 * u16; that cast is left exactly where it is, since a redundant-looking cast
 * in matched code is the kind of thing that turns out to be load bearing.
 */
extern u16 gText_DecimalDigitGlyphMap[];

/* Seeds the colour slots above for a two-choice prompt and puts the box on
 * screen: it fills the first five with 4, clears the one its argument selects,
 * marks slot 3 or slot 4 depending on that argument, then calls TextBox_Create
 * and TextBox_BuildBlocking.
 *
 * Declared here because this is the header that owns gText_abColorSlots, the
 * table it writes -- the note on that declaration already names this file as
 * one of its three writers. Its only caller, Options_Init, had the only
 * declaration. */
#endif
