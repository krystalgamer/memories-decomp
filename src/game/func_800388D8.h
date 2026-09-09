#ifndef MEMORIES_DECOMP_FUNC_800388D8_H
#define MEMORIES_DECOMP_FUNC_800388D8_H

#include "../types.h"

/* D_80090EAC entry: the fade command. Bit 6 of its operand sets D_8009B140 from
 * the D_8009AF74 pair, bit 5 sets it to 4, and bit 4 starts a fade -- white
 * through Fade_InitOutColor when bit 0 is set. The bits are tested in that
 * order and are not exclusive. */
void func_800388D8(u8 *object);

#endif
