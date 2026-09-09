#ifndef MEMORIES_DECOMP_SCRIPT_STATE_H
#define MEMORIES_DECOMP_SCRIPT_STATE_H

#include "../types.h"

/* The script engine's current command word.
 *
 * script_run_tick.c fetches it straight from the script stream
 * (D_8009B27C = *D_8009B290++) and dispatches on its low five bits
 * (D_80090C50[D_8009B27C & 0x1F]()), so the low bits carry the opcode and
 * the upper bits carry engine flags: bit 15 is the one-shot busy latch
 * documented in script_command_busy.h, and handlers set and clear 0x4000,
 * 0x2000, 0x1000, 0x800, 0x400, 0x200 and 0x80 around it.
 *
 * Two translation units deliberately keep their own spelling and must not
 * be switched to this declaration:
 *
 *   duel_effect_interaction_states.c declares it as an array plus a scalar
 *   asm() alias, because, as its own comment records, separate linker names
 *   are what stop GCC retaining the address across a call.
 *
 *   text_box_build_step.c pins it into .data with an attribute, which is
 *   what decides gp-relative versus absolute addressing under -G8.
 */
extern u16 D_8009B27C;

#endif
