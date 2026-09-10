#include "../types.h"
#include "data_80091510.h"

/* Initialized data at 0x80091510, previously a generated blob (#2602).

   Nothing reads this. Not tracked C, not the generated assembly for any
   unmatched function, and not c_symbols.ld -- before this change the only
   file in the tree mentioning D_80091510 was the blob's own extracted
   listing. Moving it here is what makes that visible; in a blob it is not.

   The name is the address on purpose. The range sits between
   gModel_abImageCopyFrameRows and gMovie_aStreamRanges and continues
   neither -- the frame table is eight bytes of a ping-pong sequence and
   this is three words -- so a neighbourhood name would imply an
   association nothing supports.

   No shape is claimed. As halfwords these would read 0x0100, 0xFFFF,
   0xFFFF and three zeros, which looks like signed pairs, but with no
   reader there is nothing to confirm that against and the word form is
   what the extractor labelled. */
u32 D_80091510[] = {
    0xFFFF0100,
    0x0000FFFF,
    0x00000000,
};
