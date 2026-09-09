#ifndef MEMORIES_DECOMP_FUNC_80028310_H
#define MEMORIES_DECOMP_FUNC_80028310_H

#include "../types.h"

/* gDuelEffect_apfnGroupHandler entry: runs the effect's dialog box. It opens
 * the box on the first frame, waits for TEXT_BOX_FLAG_DONE, then either opens
 * the choice list or -- once D_8009B248's 0x40 bit says the choice is up --
 * waits for a confirm press before destroying the box and setting 0x40 in
 * D_8009B254. */
void func_80028310(void);

#endif
