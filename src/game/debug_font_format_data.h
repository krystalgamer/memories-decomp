#ifndef MEMORIES_DECOMP_DEBUG_FONT_FORMAT_DATA_H
#define MEMORIES_DECOMP_DEBUG_FONT_FORMAT_DATA_H

#include "../types.h"

/* The two debug FntPrint format strings. debug_font_format_data.c defines
 * them as `char D_8009AF54[4] __attribute__((section(".sdata"))) = "\n";`
 * and `char D_8009AF58[4] ... = "%s\n";`, so `char` is the element type of
 * the definitions and an incomplete extern array is a compatible declaration
 * for them -- the defining unit consumes this header too.
 *
 * The array is left incomplete because every reader wants the %hi/%lo pair:
 * src/candidates_target/func_80030294.S loads both that way, and declaring
 * `char [4]` here instead -- four bytes, which is small data at -G8 -- fails
 * the build with `error: rebuilt executable is 0x1d07f4 bytes, expected
 * 0x1d0800`. That measurement is what the removed
 * DEBUG_FONT_FORMATS_AS_U8_ARRAYS guard was selecting: its `u8 []` arm and
 * this `char []` one differ only in the element type, which reaches no
 * instruction, and the tree builds byte for byte the same with one
 * declaration for both readers. func_80031084.c still carries the `#define`
 * for the removed guard; that file is in an open pull request, so deleting
 * the now-inert line is left to a follow-up. */
extern char D_8009AF54[];
extern char D_8009AF58[];

#endif
