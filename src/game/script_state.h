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

/* The script instruction cursor: a byte pointer walking the script stream.
 *
 * script_readers.c is the clearest statement of the encoding -- one reader
 * returns *D_8009B290++ for a single byte operand, and a second returns
 * current[0] | (current[1] << 8) after advancing two, so operands are
 * little-endian and the cursor is byte-granular. Command handlers advance it
 * by two, four or six according to how many operands they take, and
 * script_run_tick.c fetches the next opcode through it into D_8009B27C.
 */
extern u8 *D_8009B290;

/* The viewport tween target, X then Y, as two consecutive halfwords.
 *
 * func_8002E6B8 and Script_OpViewportTween read them out of the script
 * stream as little-endian operands (cursor[0] | (cursor[1] << 8));
 * Script_OpShowImage copies them straight into gGraphics_sViewportX/Y,
 * and Script_UpdateViewportTween interpolates the viewport towards them,
 * reading them signed for the delta (`*(s16 *)&D_8009B2A8`, retail lh)
 * and unsigned for the final copy (retail lhu). Retail reaches them
 * gp-relative in all four of those units.
 *
 * duel_effect_interaction_states.c keeps its own spelling for the same
 * reason as D_8009B27C above -- an array plus a scalar asm() alias, with
 * its comment on why -- and must not be switched to these. */
extern u16 D_8009B2A8;
extern u16 D_8009B2AA;

#endif
