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

/* The text colour slots, indexed by the low nibble of a colour command:
 * func_80038498.c reads `gText_abColorSlots[v & 0xF]`. func_800611D0.c sets
 * the first three to 4 and clears one chosen by its argument, and
 * func_8003C4E0.c walks the table from its base.
 *
 * The incomplete-array spelling every consumer already used is kept. Nothing
 * here establishes how many slots there are -- the nibble index allows
 * sixteen, but only three are ever written by name -- so no bound is claimed,
 * for the same reason the banks above carry none. */
extern u8 gText_abColorSlots[];

/* The digit glyph-index table. func_8003B5C8.c fills it, and its note there
 * says what with: each of the ten Shift-JIS digit keys is looked up in the
 * table at D_801D9004 and the 1-based match index is written here.
 * refresh_displays.c then reads it as `D_800EAFF8[gPassword_abDigits[i]]` to
 * turn an entered digit into a glyph, and func_80038148.c reads element 0 and
 * one chosen by a buffer byte.
 *
 * The incomplete-array spelling all three consumers already used is kept, and
 * for the usual reason: nothing here fixes the length. func_80038148.c writes
 * its reads as `*(u16 *)&D_800EAFF8[i]` even though the elements are already
 * u16; that cast is left exactly where it is, since a redundant-looking cast
 * in matched code is the kind of thing that turns out to be load bearing.
 */
extern u16 D_800EAFF8[];

#endif
