#ifndef MEMORIES_DECOMP_TEXT_CONSTANTS_H
#define MEMORIES_DECOMP_TEXT_CONSTANTS_H

#include "../types.h"

#define TEXT_GLOBAL_STRING_ID_BASE 0x8000
#define TEXT_BANK_ADDRESS_MASK 0xFFFF0000
#define TEXT_SINGLE_BYTE_GLYPH_LIMIT 0xF0
#define TEXT_STRING_TERMINATOR 0xFF
#define TEXT_DECIMAL_RADIX 10
#define TEXT_DECIMAL_BLANK_DIGIT 0xA

/* The three text banks the string lookup reads through.
 *
 * text_lookup_string.c resolves a string id by masking a bank's own address
 * with TEXT_BANK_ADDRESS_MASK and adding a halfword read out of it, so the
 * banks are both the addressing base and the table. That is why they are
 * declared here beside the constants those expressions use, rather than in
 * unmatched.h: they have an identified owner, and that header is for symbols
 * that are still homeless.
 *
 * All three keep the incomplete-array spelling every consumer already used.
 * The arrays are real rather than decorative -- the lookup indexes them well
 * above zero, at `index - 0xD000` and `index - TEXT_GLOBAL_STRING_ID_BASE` --
 * so the spelling is not a size claim and must not be given a bound. */
extern u16 D_801B0000[];
extern u16 D_801C0000[];
extern u16 D_801D5800[];

/* 0x8009B32E, the string id func_800383DC resolves through the three banks
 * above (duel_effect_command.c:270 reads it into `a2`). It is two bytes: D_8009B330
 * (duel_effect.h:368) starts at +2. The one loader in C, func_800383DC,
 * matched with it u16, and retail loads it lhu, gp-relative
 * (func_800383DC.s:4). FreeDuel_PlaceCursor stores into it through a named
 * address local, `slot = &D_8009B32E;` (screen_runtime.c:129-131), and
 * func_800218F0, still assembly, stores a halfword to it through $at
 * (func_800218F0.s:61-62). the-game.md:1511 calls it string ID 0x8328 + i.
 * The two units used to declare it u16 and s16. */
extern u16 D_8009B32E;

/* The text colour slots, indexed by the low nibble of a colour command:
 * func_80038498 reads `gText_abColorSlots[v & 0xF]`. func_800611D0.c sets
 * the first three to 4 and clears one chosen by its argument, and
 * func_8003C4E0 in options_screen.c walks the table from its base.
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
 * code; TextBox_BuildStep (text_box_build_step.c) reads the whole word at a
 * script byte's index and masks it with 0x8FF0FFFF. Those two read it
 * through this declaration, the u32 spelling both already used.
 * func_80039794.c reads bits 0-2 of the high halfword through its own TblEnt
 * view of the same entries and keeps that view; func_8003B5C8.c reaches
 * entry 1 under its own name, D_801D9004, with a const that its note
 * explains. 0x174 bytes to D_801D9174, 93 entries; the first word is zero in
 * the image, no C unit writes the table, and the filler was not read. Retail
 * reaches it through %hi/%lo at every site, which an incomplete array gives
 * at any -G, so no unit needs an arm. */
extern u32 D_801D9000[];

/* The digit glyph-index table. func_8003B5C8.c fills it, and its note there
 * says what with: each of the ten Shift-JIS digit keys is looked up in the
 * table at D_801D9004 and the 1-based match index is written here.
 * password/shop.c then reads it as `D_800EAFF8[gPassword_abDigits[i]]` to
 * turn an entered digit into a glyph, and func_80038148 reads element 0 and
 * one chosen by a buffer byte.
 *
 * The incomplete-array spelling all three consumers already used is kept, and
 * for the usual reason: nothing here fixes the length. func_80038148 writes
 * its reads as `*(u16 *)&D_800EAFF8[i]` even though the elements are already
 * u16; that cast is left exactly where it is, since a redundant-looking cast
 * in matched code is the kind of thing that turns out to be load bearing.
 */
extern u16 D_800EAFF8[];

/* Seeds the colour slots above for a two-choice prompt and puts the box on
 * screen: it fills the first five with 4, clears the one its argument selects,
 * marks slot 3 or slot 4 depending on that argument, then calls TextBox_Create
 * and func_80039A14.
 *
 * Declared here because this is the header that owns gText_abColorSlots, the
 * table it writes -- the note on that declaration already names this file as
 * one of its three writers. Its only caller, Options_Init, had the only
 * declaration. */
void func_8003C4E0(s32 slot);

#endif
