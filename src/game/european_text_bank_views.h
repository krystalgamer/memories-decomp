#ifndef MEMORIES_DECOMP_EUROPEAN_TEXT_BANK_VIEWS_H
#define MEMORIES_DECOMP_EUROPEAN_TEXT_BANK_VIEWS_H

#include "../types.h"

#ifdef VERSION_EUROPE
/* Stored boot-tail destinations, not the text bytes or table declarations. */
extern u8 *G32 gFile_pBootTextStage5Destination __asm__("D_800101E0");
extern u8 *G32 gFile_pBootTextStage6Destination __asm__("D_800101E4");
extern u8 *G32 gFile_pBootTextStage7Destination __asm__("D_800101E8");

extern u16 gText_AlternateStringAddressAnchor[] __asm__("D_801C0004");
extern u8 gText_DecimalDigitGlyphString[] __asm__("D_801BF870");
#endif

#endif
