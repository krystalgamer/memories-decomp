#ifndef MEMORIES_DECOMP_EUROPEAN_TEXT_BANK_VIEWS_H
#define MEMORIES_DECOMP_EUROPEAN_TEXT_BANK_VIEWS_H

#include "../types.h"

#ifdef VERSION_EUROPE
/* Stored boot-tail destinations, not the text bytes or table declarations.
 * The European boot package's last three stages load to the addresses held
 * here: 0x801B0000, 0x801C0000 and 0x801D5800, the European text banks. */
extern u8 *G32 gFile_pBootTextStage5Destination;
extern u8 *G32 gFile_pBootTextStage6Destination;
extern u8 *G32 gFile_pBootTextStage7Destination;

extern u16 gText_AlternateStringAddressAnchor[];
extern u8 gText_DecimalDigitGlyphString[];
#endif

#endif
