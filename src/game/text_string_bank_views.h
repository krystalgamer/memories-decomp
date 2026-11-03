#ifndef GAME_TEXT_STRING_BANK_VIEWS_H
#define GAME_TEXT_STRING_BANK_VIEWS_H

#include "../types.h"

/* The three North American text banks the string lookup reads through.
 * The Japanese lookup selects gText_GlobalStringOffsets for ids with bit 15 set and
 * gText_PrimaryStringOffsets otherwise, using a narrower bank-address mask.
 *
 * text_lookup_string.c resolves a string id by masking a bank's own address
 * with TEXT_BANK_ADDRESS_MASK and ORing a halfword read out of it, so the
 * banks are both the addressing base and the table.
 *
 * All three keep the incomplete-array spelling every consumer already used.
 * The arrays are real rather than decorative -- the lookup indexes them well
 * above zero, at `index - 0xD000` and `index - TEXT_GLOBAL_STRING_ID_BASE` --
 * so the spelling is not a size claim and must not be given a bound. */
extern u16 gText_PrimaryStringOffsets[];
extern u16 gText_GlobalStringOffsets[];
extern u16 gText_PrimaryStringAddressAnchor[];

#endif
