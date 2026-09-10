#ifndef MEMORIES_DECOMP_FUNC_8002BAB4_H
#define MEMORIES_DECOMP_FUNC_8002BAB4_H

#include "../types.h"

typedef struct {
    s32 values[4];
} LibraryViewQuad;

typedef char LibraryViewQuad_size_must_be_0x10[
    sizeof(LibraryViewQuad) == 0x10 ? 1 : -1
];

/* The library screen's state record, as the two units that reach it through
 * its leading byte spell it. func_8002BAB4 dispatches on `D_800EA1E8[0] & 0xF`
 * -- its own comment calls that the low nibble of the first byte -- and
 * func_8002BFCC clears the byte when it re-enters the screen, both after
 * taking the base into a u8 * of their own.
 *
 * func_80029590.c and func_8002A3CC.c reach the same address through a typed
 * LibraryMotionState view instead, and neither includes this header, so the
 * two spellings never meet and no guarded arm is needed.
 *
 * The two views are not merged here. Whether that mode byte is a field of the
 * motion record or a separate object sharing its first bytes is not
 * established, and either declaration reaching the other pair would assert
 * that it is. */
extern u8 D_800EA1E8[];

void func_8002BAB4(void);

#endif
