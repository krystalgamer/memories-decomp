#ifndef MEMORIES_DECOMP_SCRIPT_OP_SHOW_DIALOG_H
#define MEMORIES_DECOMP_SCRIPT_OP_SHOW_DIALOG_H

#include "../types.h"

/* D_80090C50 handler: opens a dialog text box at a fixed 0x10,0xB0 rectangle
 * from the 16-bit string id in the stream, marks it through
 * DuelEffect_MarkObjectIfActive and sets its 0x8 flag. Bit 15 of the id
 * instead raises 0x4000 in D_8009B27C and CLEARS that 0x8 bit, reading the
 * box's flags word through a volatile view so the two writes are not merged.
 * The busy word is then latched into D_8009B28C, which this unit defines. */
void Script_OpShowDialog(void);

/* The latched busy word. This unit owns the definition; script_run_tick.c
 * clears it, tests it, and copies it back into D_8009B27C, and spelled its
 * own `extern u16` for it.
 *
 * The note here used to say the symbol was defined rather than declared "so
 * the store stays gp-relative", and that a header declaration would disturb
 * it. notes/build.md records that this was wrong: a plain declaration in
 * front of the definition does not change it, and the test is agreement
 * rather than definition. The consumer's spelling and the definition are the
 * same `u16`, so the declaration is inert and belongs here. */
extern u16 D_8009B28C;

#endif
